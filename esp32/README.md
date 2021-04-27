
states:
- (W) waiting
- (M) moving piece
- (C) castling
- (P) en passant
- (T) taking piece
- (I) invalid state

transitions:
- (W) -- piece removed           -> (M)
- (W) -- invalid change          -> (I)

- (M) -- piece placed            -> (W)
- (M) -- king placed in castling -> (C)
- (M) -- en passant              -> (P)
- (M) -- piece removed           -> (T)
- (M) -- invalid change          -> (I)

- (C) -- rook placed in castling -> (W)
- (C) -- invalid state           -> (C)

- (P) -- pawn removed            -> (W)
- (P) -- invalid state           -> (P)

- (T) -- piece placed            -> (W)
- (T) -- invalid change          -> (I)

- (I) -- invalid state           -> (I)
- (I) -- valid state             -> (W)
