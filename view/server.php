<?php

// 设置 CORS 头部
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type, Authorization');

// 设置文件路径
$file = $_GET['file'] ?? '';

// 检查文件是否存在
if (!file_exists($file)) {
    die('File not found');
}

// 设置适当的 HTTP 头部
if (strpos($file, ".wasm") !== false) {
    header('Content-Type: application/wasm');
}
else {
    header('Content-Type: application/octet-stream');
}

header('Content-Description: File Transfer');
header('Content-Disposition: attachment; filename="' . basename($file) . '"');
header('Expires: 0');
header('Cache-Control: no-cache, no-store, must-revalidate');
header('Pragma: no-cache');
header('Content-Length: ' . filesize($file));

// 清空输出缓冲区
ob_clean();
flush();

// 输出文件内容
readfile($file);
exit;

?>