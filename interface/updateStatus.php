<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $content = file_get_contents('php://input');
    $decoded = json_decode($content, true);

    $currentData = json_decode(file_get_contents('status.json'), true);

    foreach ($decoded as $key => $value) {
        $currentData[$key] = $value;
    }

    file_put_contents('status.json', json_encode($currentData, JSON_PRETTY_PRINT));
    echo json_encode(["message" => "Success"]);
}
