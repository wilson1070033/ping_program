# C++ ICMP Ping 工具

## 專案概述

這是一個使用 C++ 開發的專業級 ICMP（Internet Control Message Protocol）ping 工具，採用原始通訊端（Raw Socket）技術實作網路診斷功能。本工具提供詳細的封包分析與統計資訊，適用於網路除錯、效能監控與教學研究用途。

---

## 核心功能

### 網路診斷功能
- **主機名稱解析**：支援 IPv4 位址或完整網域名稱（FQDN）
- **ICMP 封包處理**：手動構建 ICMP Echo Request 封包，包含完整的標頭欄位與校驗和計算
- **來回時間測量**：精確計算封包往返時間（Round-Trip Time, RTT），精度達微秒級

### 詳細除錯輸出
- **通訊端管理追蹤**：顯示通訊端建立、設定與關閉的完整流程
- **封包準備資訊**：展示 ICMP 標頭各欄位（Type、Code、ID、Sequence、Checksum）
- **傳輸狀態監控**：即時回報封包傳送與接收狀態
- **標頭深度解析**：
  - IP 標頭分析（版本、標頭長度、TTL、協定、總長度）
  - ICMP 標頭分析（類型、代碼、識別碼、序號、校驗和）
- **封包驗證邏輯**：顯示完整的封包匹配過程（類型、ID、序號驗證）

### 統計分析功能
- **基礎封包統計**
  - 已傳送封包數量
  - 已接收封包數量
  - 封包遺失數量與遺失率（百分比）
  - 錯誤計數

- **RTT 時間統計**
  - 最小來回時間（Min RTT）
  - 平均來回時間（Average RTT）
  - 最大來回時間（Max RTT）
  - 標準差（Standard Deviation）
  - 全距（Range）

- **RTT 分佈分析**
  - 顯示每個封包的個別 RTT 值
  - 提供完整的測試持續時間資訊

---

## 技術規格

### 開發環境
- **程式語言**：C++（符合 C++11 標準）
- **編譯器**：GCC 4.8+ 或 Clang 3.4+
- **作業系統**：Linux、macOS 或其他支援 BSD Socket 的 Unix-like 系統

### 網路協定
- **協定層級**：網路層（Network Layer）
- **使用協定**：ICMP（Internet Control Message Protocol）
- **封包類型**：Echo Request（Type 8）/ Echo Reply（Type 0）
- **通訊端類型**：SOCK_RAW（原始通訊端）

### 權限需求
- **執行權限**：需要 root 或 sudo 權限
- **原因**：原始通訊端操作需要系統管理員權限

---

## 系統需求

### 必要條件
- C++ 編譯器（g++ 或 clang++）
- Unix-like 作業系統（Linux 或 macOS）
- Root 權限（用於執行程式）
- 標準 C++ 函式庫

### 相依函式庫
- `libm`（數學函式庫，用於標準差計算）
- 系統標準標頭檔：
  - `<sys/socket.h>`
  - `<netinet/ip.h>`
  - `<netinet/ip_icmp.h>`
  - `<arpa/inet.h>`

---

## 編譯指南

### 基本編譯

使用以下指令編譯專案：

```bash
g++ -o ping main.cpp PingClient.cpp ICMPPacket.cpp PingStatistics.cpp utils.cpp -lm
```

### 編譯參數說明

- **`-o ping`**：指定輸出檔案名稱為 `ping`
- **`-lm`**：連結數學函式庫（用於 `sqrt()` 函式）

### 最佳化編譯

若需要最佳化版本，可加入最佳化旗標：

```bash
g++ -o ping -O2 -Wall -Wextra main.cpp PingClient.cpp ICMPPacket.cpp PingStatistics.cpp utils.cpp -lm
```

參數說明：
- **`-O2`**：啟用第二級最佳化
- **`-Wall`**：啟用所有警告訊息
- **`-Wextra`**：啟用額外警告訊息

---

## 使用說明

### 基本語法

```bash
sudo ./ping <目標主機> [封包數量]
```

### 參數說明

- **`<目標主機>`**（必要）：目標 IP 位址或主機名稱
- **`[封包數量]`**（選用）：要傳送的封包數量，預設值為 4

### 使用範例

#### 範例一：Ping Google 預設次數

```bash
sudo ./ping google.com
```

#### 範例二：Ping 本機 10 次

```bash
sudo ./ping 127.0.0.1 10
```

#### 範例三：Ping 特定 IP 位址

```bash
sudo ./ping 8.8.8.8 5
```

### 執行權限說明

由於程式使用原始通訊端（`SOCK_RAW`），必須以 root 權限執行：

- **Linux/macOS**：使用 `sudo` 前綴指令
- **權限不足時**：程式會顯示錯誤訊息並說明需要 root 權限

---

## 程式碼架構

### 專案結構

本專案採用物件導向設計，將功能模組化為多個類別：

```
專案根目錄/
├── main.cpp                  # 程式進入點
├── PingClient.hpp            # Ping 客戶端類別標頭檔
├── PingClient.cpp            # Ping 客戶端類別實作
├── ICMPPacket.hpp            # ICMP 封包類別標頭檔
├── ICMPPacket.cpp            # ICMP 封包類別實作
├── PingStatistics.hpp        # 統計類別標頭檔
├── PingStatistics.cpp        # 統計類別實作
├── utils.hpp                 # 工具函式標頭檔
├── utils.cpp                 # 工具函式實作
└── README.md                 # 專案說明文件
```

### 核心類別說明

#### **PingClient 類別**
- **職責**：核心控制器，負責協調整個 ping 流程
- **主要功能**：
  - 建立與管理原始通訊端
  - DNS 主機名稱解析
  - 封包傳送與接收協調
  - 逾時控制與錯誤處理
  - 統計資料收集

#### **ICMPPacket 類別**
- **職責**：ICMP 封包的建立與處理
- **主要功能**：
  - 構建 ICMP Echo Request 封包
  - 填充封包資料區段
  - 計算並設定校驗和（Checksum）
  - 提供封包資料存取介面

#### **PingStatistics 類別**
- **職責**：統計資訊的收集與分析
- **主要功能**：
  - 追蹤已傳送/已接收封包數量
  - 記錄每次 ping 的 RTT 值
  - 計算統計指標（最小值、最大值、平均值、標準差）
  - 產生詳細的統計報告

#### **utils 模組**
- **職責**：提供格式化輸出與輔助功能
- **主要功能**：
  - 格式化輸出函式（標題、區段、資訊列印）
  - 時間戳記產生
  - ICMP 類型名稱轉換

---

## 輸出範例

### 執行輸出結構

程式輸出分為以下區段：

#### **1. 初始化階段**
```
================================================================================
  ICMP PING - VERBOSE MODE
================================================================================

  Program Version          : 1.0
  Target Host              : google.com
  Process ID               : 12345
  Timeout                  : 2 seconds
```

#### **2. 通訊端建立**
```
--------------------------------------------------------------------------------
  SOCKET CREATION
--------------------------------------------------------------------------------
  Creating raw socket...
  [SUCCESS] Socket created
  Socket File Descriptor   : 3
  Socket Type              : SOCK_RAW
  Protocol                 : IPPROTO_ICMP
```

#### **3. 封包傳輸**
```
================================================================================
  PACKET 1 OF 4
================================================================================

--------------------------------------------------------------------------------
  PACKET PREPARATION
--------------------------------------------------------------------------------
  ICMP Type                : Echo Request (8)
  ICMP Code                : 0
  Process ID               : 12345
  Sequence Number          : 1
  Checksum                 : 0xa3f4
```

#### **4. 封包接收與分析**
```
--------------------------------------------------------------------------------
  PACKET RECEPTION
--------------------------------------------------------------------------------
  Waiting for reply...
  [RECEIVED] Packet received
  Bytes Received           : 84
  
  IP HEADER ANALYSIS:
  IP Version               : 4
  Time To Live (TTL)       : 56
  
  ICMP HEADER ANALYSIS:
  ICMP Type                : 0 (Echo Reply)
  ICMP Sequence            : 1
```

#### **5. 統計報告**
```
================================================================================
  PING STATISTICS
================================================================================
  Target Host              : google.com
  Test Duration            : 4.125 seconds

--------------------------------------------------------------------------------
  PACKET STATISTICS
--------------------------------------------------------------------------------
  Packets Transmitted      : 4
  Packets Received         : 4
  Packet Loss Rate         : 0 %

--------------------------------------------------------------------------------
  ROUND-TRIP TIME (RTT) STATISTICS
--------------------------------------------------------------------------------
  Minimum RTT              : 15.234 ms
  Average RTT              : 18.567 ms
  Maximum RTT              : 24.891 ms
  Standard Deviation       : 3.425 ms
```

---

## 注意事項

### 權限相關
- 程式必須以 root 權限執行，否則無法建立原始通訊端
- 在生產環境中使用時，建議檢查安全性政策

### 網路環境
- 某些網路環境可能會封鎖 ICMP 封包
- 防火牆設定可能影響程式執行結果
- 本機迴環（loopback）測試可能會接收到自己傳送的 Echo Request

### 效能考量
- 預設逾時時間為 2 秒，可在 `PingClient` 建構子中調整
- 封包間隔固定為 1 秒，適合一般網路診斷用途
- 大量封包傳送可能影響網路效能

### 平台相容性
- 程式碼使用 POSIX 標準 API，應可在大多數 Unix-like 系統上執行
- Windows 系統需要使用不同的通訊端 API（Winsock），本專案不支援

---

## 錯誤排除

### 常見錯誤與解決方案

#### **錯誤：Permission denied**
```
[FAILED] Cannot create socket
Error Message: Permission denied
```
**解決方案**：使用 `sudo` 執行程式

#### **錯誤：Name or service not known**
```
[FAILED] Cannot resolve hostname
```
**解決方案**：
- 檢查主機名稱拼寫是否正確
- 確認 DNS 設定是否正常
- 嘗試使用 IP 位址代替主機名稱

#### **錯誤：Network unreachable**
**解決方案**：
- 檢查網路連線狀態
- 確認路由設定是否正確
- 驗證防火牆規則

---

## 技術細節

### ICMP 封包結構

本工具實作的 ICMP Echo Request 封包結構：

```
ICMP 標頭（8 bytes）：
├── Type (1 byte)        : 8（Echo Request）
├── Code (1 byte)        : 0
├── Checksum (2 bytes)   : 計算值
├── Identifier (2 bytes) : Process ID
└── Sequence (2 bytes)   : 序號（遞增）

資料區段（56 bytes）：
└── 填充資料（0x20-0x57）
```

### 校驗和計算

ICMP 校驗和計算採用網際網路校驗和（Internet Checksum）演算法：

1. 將封包視為 16-bit 整數陣列
2. 對所有 16-bit 值進行求和
3. 處理溢位的進位
4. 對結果取一補數（One's Complement）

### RTT 計算方法

來回時間（RTT）計算公式：

```
RTT = (接收時間 - 傳送時間)
    = (recv_tv.tv_sec - send_tv.tv_sec) × 1000.0 +
      (recv_tv.tv_usec - send_tv.tv_usec) / 1000.0
```

單位：毫秒（ms）

---

## 授權資訊

本專案為教學與研究用途開發，使用者可自由修改與使用。

---

## 版本歷史

- **v1.0**（當前版本）
  - 實作基本 ICMP ping 功能
  - 提供詳細的除錯輸出
  - 完整的統計分析功能
  - 支援主機名稱解析

---

## 聯絡資訊

如有技術問題或建議，請透過專案管理系統回報。

---

**文件最後更新**：2025 年 11 月
