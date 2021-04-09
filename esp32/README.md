
states:
- (W) waiting
- (M) moving piece
- (C) castling
- (T) taking piece
- (I) invalid state

transitions:
- (W) -- piece removed           -> (M)
- (W) -- invalid change          -> (I)

- (M) -- piece placed            -> (W)
- (M) -- king placed in castling -> (C)
- (M) -- piece removed           -> (T)
- (M) -- invalid change          -> (I)

- (C) -- rook placed in castling -> (W)
- (C) -- invalid state           -> (C)

- (T) -- piece placed            -> (W)
- (T) -- invalid change          -> (I) 

- (I) -- invalid state           -> (I)
- (I) -- valid state             -> (W)
