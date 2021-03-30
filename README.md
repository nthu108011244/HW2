# HW2
因為 cut-off frequency = 55 (Hz)， 所以設定了 [1, 5, 10, 20 ,30 ,40, 50, 60, 70, 80, 90, 100] 不同頻率的波型供使用者選擇。\
因為 (1 + 0 + 8 + 1 + 1 + 2 + 4 + 4) mod(10) = 1， 所以波型將在 1/10 週期的位置達到最大值。

# uLED使用者介面
總共有兩種模式\
● 頻率選擇模式：主要功能為頻率選擇，有「Down」、「Select」、「Up」三個按鍵，「Down」可以調低頻率；「Up」可以調高頻率；「Select」可以鎖定頻率並進入波型生成模式。\
● 波型生成模式：主要功能為生成波型，僅有一個「Stop」鍵，可以暫停生成波型並回到頻率選擇模式重新選擇欲生成的頻率。

# 波型生成原理
在mian fuction中的無窮迴圈不斷呼叫wave_generate()，每呼叫一次wave_generate()會完整輸出一次波型，\
而在wave_generate()中固定約每100us改變一次輸出電壓，並根據計算得出的公式：\
● 前1/10週期每次增加 （9 * 頻率 / 1000）。\
● 後9/10週期每次減少 （頻率 / 1000）。

# 波型取樣&FFT原理
宣告以下Thread以及EventQueue\
Thread sampleThread;\
Thread screenThread;\
EventQueue sampleQueue;\
EventQueue screenQueue;\
配合以下兩個function分別用以取樣和輸出樣本\
void wave_sample();\
void sample_print();\
由於波型頻率在1 ~ 100不等，因此統一sampling rate = 200 (1/s)，以確保每一種波型取樣後皆不會失真，\
在進入波型生成模式時，呼叫sampleQueue.call(&wave_sample)進行第一次取樣，\
待取樣足夠樣本數後，在wave_sample()中呼叫printQueue.call(&sample_print)，將取樣結果輸出在/dev/ttyACM* 上，讓FFT.py讀取數據作傅立葉分析，\
由於輸出時波型不穩定，，因此等到所有樣本輸出完後，等候3秒，在sample_print中呼叫sampleQueue.call(&wave_sample)進行下一次的取樣，以確保波型為我們所以要求的，\
在此形成一個 (取樣) -> (輸出樣本) -> (靜待3秒) -> (取樣) -> (輸出樣本) -> (靜待3秒) -> (取樣) -> .......的輪迴。

# 結果呈現
1Hz:\
![image](https://user-images.githubusercontent.com/79581483/113017000-9b8ed480-91b1-11eb-88b2-26b39c54417a.png)
50Hz:\
![image](https://user-images.githubusercontent.com/79581483/113017114-b6614900-91b1-11eb-8b62-abf60fcadd8c.png)
90Hz:\
![image](https://user-images.githubusercontent.com/79581483/113017149-bf521a80-91b1-11eb-8f3f-f7cc211d4945.png)
