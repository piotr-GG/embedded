     wifi.setmode(wifi.STATION);
     cfg={}
     cfg.ssid="*****"
     cfg.pwd="*****"
     wifi.sta.config(cfg)

     gpio.mode(0, gpio.OUTPUT)
     gpio.write(0, gpio.HIGH)

     gpio.mode(8, gpio.OUTPUT)
     gpio.write(8, gpio.LOW)   

     gpio.mode(2, gpio.OUTPUT) 
     gpio.write(2, gpio.HIGH) -- RELAY

     gpio.mode(5,gpio.INT) --Hallotron
     
     ds18b20.setup(3);
     print("PWM Function test")
     pwm.setup(6,1000,1023);
     pwm.start(6);
    
    
     local r_min = 700;
     local r_max = 900;
     local temp_reg = 0;
     local auto_mode = 0; 
     local r= 900;
     local flag=1;
     local counter = 0;
     local last_count = 0;
     local relay = 1;
     local temperature;
     local voltage = 0;

     local function pin1cb(level, pulse2)
        counter = counter + 1
     end
     
     gpio.trig(5, "down", pin1cb)
     
     tmr.alarm(2,1000,1,function()
         if(auto_mode == 1) then         --AUTO MODE
            autoRegulation()
         end 
        
         if(wifi.sta.getip() ~= nil) then
            gpio.write(0, gpio.LOW)
         else 
            gpio.write(0, gpio.HIGH)
         end
         
         pwm.setduty(6,r);
         print("r = " .. r)
         print("mode = " .. auto_mode)
         print("Relay = " .. relay)
         ds18b20.read(
            function(ind,rom,res,temp,tdec,par)
                if(temp ~= 85) then
                    temperature = temp
                    print("Temperature = " .. temp)
                end
            end,{});
         last_count = counter/2   
         print("Counter = " .. last_count)
         
         counter = 0
         voltage = ((11*r)/1000)+15802/10000
         print("Voltage = " .. voltage)
     end)


   
   function autoRegulation() 
        local delta_temp = temperature - temp_reg
        if(delta_temp > 10) then
            r= 1023
        elseif(delta_temp <=10 and delta_temp >5) then
            r = 900
        elseif (delta_temp <= 5 and delta_temp >0) then
            r = 800
        elseif (delta_temp <=0) then 
            r = 0
        end
    end

    function postThingSpeak(level)
        connout = nil
        connout = net.createConnection(net.TCP, 0)
 
        connout:on("receive", function(connout, payloadout)
            if (string.find(payloadout, "Status: 200 OK") ~= nil) then
                print("Posted OK");
            end
        end)
 
        connout:on("connection", function(connout, payloadout)
 
        print ("Posting...");   
 
        connout:send("GET /update?api_key=GMDJUSM3BXV3UHRA&field1=" .. temperature .. "&field2=".. r .. "&field3=".. last_count .. "&field4=".. relay .. "&field5=".. voltage
        .. " HTTP/1.1\r\n"
        .. "Host: api.thingspeak.com\r\n"
        .. "Connection: close\r\n"
        .. "Accept: */*\r\n"
        .. "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n"
        .. "\r\n")
    end)
 
    connout:on("disconnection", function(connout, payloadout)
        --connout:close();
        collectgarbage();
    end)
 
    connout:connect(80,'api.thingspeak.com')
end
 
    tmr.alarm(1, 15000, 1, function() postThingSpeak(0) end)

srv=net.createServer(net.TCP) 
srv:listen(80,function(conn) 
    conn:on("receive", function(client,request)
        print("RECEIVE DATA....")
        gpio.write(8,gpio.HIGH)
        tmr.create():alarm(3000, tmr.ALARM_SINGLE, function()
            gpio.write(8,gpio.LOW)
        end)
        
        if(string.find(request,"RELAY#ON") ~= nil)then
            relay = 1
            gpio.write(2, gpio.HIGH)
        elseif(string.find(request,"RELAY#OFF") ~= nil)then
            relay = 0
            gpio.write(2, gpio.LOW)
        end

        if(string.find(request,"PWM") ~= nil)then
            print("PWM Duty Cycle: " .. string.sub(request, (string.find(request, "#")+1)))
            r = tonumber(string.sub(request, (string.find(request, "#")+1)))
        end

        if(string.find(request,"AUTO") ~= nil)then
            print("Set temperature: " .. string.sub(request, (string.find(request, "#")+1))) 
            auto_mode = 1
            temp_reg = tonumber(string.sub(request, (string.find(request, "#")+1)))
        end

        if(string.find(request,"MANUAL") ~= nil)then
            print("Set RPM Value: " .. string.sub(request, (string.find(request, "#")+1))) 
            auto_mode = 0
            r = tonumber(string.sub(request, (string.find(request, "#")+1)))
        end
        --client:close();
        collectgarbage();
    end)
end)
