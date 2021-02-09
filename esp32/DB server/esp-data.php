<!DOCTYPE html>
<html><body>
<?php

$servername = "localhost";

// nazwa bazy danych
$dbname = "id13373375_esp32";
// nazwa uzytkownika
$username = "id13373375_esp32_db";
// haslo
$password = "zvf~X<Pl&54T~{%F";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, sensor, value1, value2, timestamp FROM SensorData ORDER BY id DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>Sensor</td> 
        <td>Abs humidity</td> 
        <td>Dew point</td>
        <td>Timestamp</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_sensor = $row["sensor"];
        $row_value1 = $row["value1"];
        $row_value2 = $row["value2"]; 
        $row_reading_time = $row["timestamp"];

        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_sensor . '</td> 
                <td>' . $row_value1 . '</td> 
                <td>' . $row_value2 . '</td>
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>
