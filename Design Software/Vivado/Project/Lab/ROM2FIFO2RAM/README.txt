Problem:
1. IDLE 沒有考慮 FIFO 初始時內有其他非相關資料的問題
2. Timing Constraints 未加入 set_clocks_group 或是 set_false_path
3. 不要拿資料做 if else 的判斷標準