<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $jsonContent = file_get_contents('php://input');
    $data = json_decode($jsonContent, true);

    if ($data) {
        file_put_contents('input.json', json_encode($data, JSON_PRETTY_PRINT));
        echo "Data Received and Stored";
    } else {
        echo "Invalid JSON Data";
    }
} else {
    echo "Invalid Request Method";
}
?>
