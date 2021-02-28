
states:
- (W) waiting, board, pieces allowed to be removed
- (M) moving piece, board, pieces allowed to be removed, pieces allowed to be placed
- (C) castling, board, 
- (T) taking piece, board, pieces allowed to be placed
- (I) invalid state, board

transitions:

- (W) -- piece removed           -> (M)
- (W) -- invalid change          -> (I)

- (M) -- piece placed            -> (W)
- (M) -- king placed in castling -> (C)
- (M) -- piece removed           -> (T)
- (M) -- invalid change          -> (I)

- (C) -- rook placed in castling -> (W)
- (C) -- invalid change          -> (I)

- (T) -- piece placed            -> (W)
- (T) -- invalid change          -> (I) 

- (I) -- new board invalid       -> (I)
- (I) -- new board valid         -> (W)

(last board, new board) -> piece placed | piece removed | invalid change
