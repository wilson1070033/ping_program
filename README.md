C++ 詳細 Ping 工具 (Verbose Ping)

這是一個使用 C++ 實作的 ICMP ping 工具。它使用原始通訊端 (Raw Sockets) 來傳送 ICMP Echo Request 封包，並接收 Echo Reply 封包。

此工具的主要特色是其「詳細模式」(Verbose Mode) 輸出，它會顯示封包準備、傳輸、接收和標頭分析的每一步，非常適合用於學習網路程式設計和 ICMP 協定。

功能特色

主機名稱解析：可接受 IP 位址或主機名稱 (例如 google.com)。

ICMP 封包處理：手動建立 ICMP 標頭並計算校驗和 (Checksum)。

詳細的除錯輸出：

顯示通訊端建立和設定的過程。

顯示封包準備的詳細資訊 (Type, Code, ID, Sequence, Checksum)。

顯示封包傳輸的狀態。

在接收時，詳細分析傳入封包的 IP 標頭 (TTL, Protocol 等) 和 ICMP 標頭。

顯示封包驗證 logique (比對 Type, ID, Sequence)。

完整的統計報告：

封包統計：已傳送、已接收、已遺失、遺失率 (%)。

RTT 統計：最小、平均、最大來回時間 (ms)。

進階統計：計算 RTT 的標準差 (Standard Deviation) 和全距 (Range)。

RTT 分佈：列出每一次成功 ping 的 RTT 值。

需求

一個 C++ 編譯器 (例如 g++ 或 clang++)。

Linux, macOS 或其他支援 BSD sockets 的 Unix-like 系統。

執行時需要 Root (sudo) 權限，因為程式使用了 SOCK_RAW 原始通訊端。

如何編譯

您可以使用 g++ 來編譯此程式。因為我們已經將程式碼拆分為多個 .cpp 檔案，所以需要將它們全部納入編譯指令中。

我們仍需要連結 m 函式庫 (math library) 因為 PingStatistics 中使用了 sqrt 來計算標準差。

g++ -o ping main.cpp PingClient.cpp ICMPPacket.cpp PingStatistics.cpp utils.cpp -lm


如何執行

由於需要 root 權限，請使用 sudo 來執行。

語法：

sudo ./ping <hostname or IP address> [count]


<hostname or IP address>：您想 ping 的目標。

[count]：(選用) 您想傳送的封包數量。預設為 4。

範例

Ping Google (預設 4 個封包):

sudo ./ping google.com


Ping 本機 (localhost) 10 次:

sudo ./ping 127.0.0.1 10


程式碼結構

程式碼透過物件導向的方式組織：

main.cpp: 程式進入點，負責解析命令列參數並啟動 PingClient。

PingClient (.hpp/.cpp): 核心類別，負責管理通訊端、主機解析、收發迴圈以及協調其他元件。

ICMPPacket (.hpp/.cpp): 負責建立、準備 ICMP Echo Request 封包，包括資料填充和校驗和計算。

PingStatistics (.hpp/.cpp): 負責追蹤所有已傳送/已接收的封包，並在程式結束時計算和顯示詳細的統計報告。

utils (.hpp/.cpp): 提供格式化的輸出功能 (例如 printHeader, printSection, printInfo)。
