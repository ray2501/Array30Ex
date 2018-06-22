Array30Ex
=====

行列三十輸入法是一個**開放授權**的輸入法，因此在 Windows 2000/Windows XP 中已經有內建， 同時有興趣移植到其它系統的人也可以依自己的興趣進行移植。

Array30Ex 是一個一般執行檔層級的行列三十輸入法，不用安裝就可以使用，實作的目的是當使用者沒有安裝的權限時，有一個輸入工具可以使用。

實作方法：使用 Keyboard hook 來取得使用者的鍵盤輸入，再依據行列的規格書實作。 

![Array30Ex.png](https://github.com/ray2501/Array30Ex/blob/master/Array30Ex.png)


* 基本輸入：使用剪貼薄送字（SendInput 送出 SHIFT+INSERT / CTRL + V）、使用 WM_IME_CHAR 訊息
* 支援 United States - Dvorak keyboard （使用 QWERTY - Dvorak 對應的方式）
* 送字時有自動切換功能（預設是開啟的）
     * 啟動後如果遇到 Edit/RichEdit Control 元件，會自動切換到使用插入方法送字，
       這樣可以增加執行時的效率（因為使用剪貼簿會使用比這個方法使用更多的系統資源）。
     * 在開啟自動切換送字功能後，除了使用插入方法到微軟提供的 Edit/RichEdit Control 元件這個方式外，
       使用者可以在 listclass.txt/listtitle.txt 自行設定當開啟自動切換送字功能時要切換的送字方式。設定後重開 Array30Ex 設定值就會生效。
     * 下面是輸入模式的設定值：  
         1. 使用 WM_IME_CHAR 訊息  
         2. 使用剪貼薄 (SHIFT+INSERT) 送字  
         3. 使用剪貼薄 (CTRL + V) 送字  
         4. 使用插入方法到微軟提供的 Edit/RichEdit Control 元件  
         5. 使用 GTK+ 所撰寫的程式  
    * 一級簡碼和二級簡碼支援
    * 支援 2^ (W) 符號輸入，並且支援 SHIFT + < 與 SHIFT + >
    * 使用詞庫支援日文假名輸入（only support Unicode）
    * 支援基本的 * 與 ? 的萬用字元搜尋方式（不支援在詞上面使用此功能）
    * 使用 CTRL+SPACE 切換行列/英數模式，使用 SHIFT+SPACE 切換全形/半形模式
    * Shift 切換中/英文輸入
    * 實作縮小到 System Tray功能，使用 SHIFT+ F8 隱藏視窗，SHIFT+F9 恢復視窗
    * 實作基本的繁簡即時轉換功能
    * 可使用支援到 Unicode 3.1 的對照表
    * 實作下列特別編碼
        * 「，，ＯＰＴ」 開啟設定對話框
        * 「，，ＰＬＣ」 輸入法列回到預設位置
        * 「，，ＯＵＴ」 離開

* 會記錄使用者使用滑鼠移動視窗後的位置
* 右鍵選單實作「設定視窗透明度」選項，可以在程式執行期間動態改變視窗透明度。 

編譯工具：
* MinGW 

支援平台：
* Windows 2000/XP

相關連結
* [行列輸入法的家](https://www.facebook.com/array.com.tw/)
* [行列輸入法 on 維基百科](http://zh.wikipedia.org/wiki/%E8%A1%8C%E5%88%97%E8%BC%B8%E5%85%A5%E6%B3%95)
