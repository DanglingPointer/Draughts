// Rules: https://en.wikipedia.org/wiki/English_draughts
// For console version the macro CONSOLE must be defined
using System;
using System.Diagnostics;
using System.Collections.Generic;

namespace Checkers
{
    //===============================================================================
    internal static class C
    {
        /// <summary> Size of the board's side </summary>
        public static uint BoardSize
        {
            get { return m_Size; }
            set { m_Size = value; m_Length = value * value / 2; }
        }
        public static uint ArrayLength
        {
            get { return m_Length; }
        }
        private static uint m_Size = 8;
        private static uint m_Length = 32;
        /// <summary> Substitution for #define WHITE true </summary>
        public const bool White = true;
        /// <summary> Substitution for #define BLACK false </summary>
        public const bool Black = false;
        /// <summary> Substitution for #define KING true </summary>
        public const bool King = true;
        /// <summary> Substitution for #define MAN false </summary>
        public const bool Man = false;
    }
    [Flags]
    public enum Piece : byte
    {
        Empty = 0x0,
        White = 0x1,
        Black = 0x2,
        King  = 0x4
    }
    [Flags]
    public enum Direction : byte
    {
        None        = 0x0,
        RightUp     = 0x1,
        LeftUp      = 0x2,
        RightDown   = 0x4,
        LeftDown    = 0x8
    }
    public struct Position
    {
        public Position(uint row, uint col)
        {
            if (row >= C.BoardSize || col >= C.BoardSize)
                throw new ArgumentOutOfRangeException();
            Row = row;
            Col = col;
        }
        public readonly uint Row;
        public readonly uint Col;
        /// <summary> Returns new position with given offset from 'this'. </summary>
        public Position Offset(int rowoffset, int coloffset)
        {
            int row = Convert.ToInt32(Row) + rowoffset;
            int col = Convert.ToInt32(Col) + coloffset;
            return new Position(Convert.ToUInt32(row), Convert.ToUInt32(col));
        }
        /// <summary> Converts position to array index </summary>
        public static implicit operator uint(Position p)
        {
            return p.Row * (C.BoardSize / 2) + p.Col / 2;
        }
        /// <summary> Converts array index to position </summary>
        public static implicit operator Position(uint index)
        {
            uint row = index / (C.BoardSize / 2);
            uint col = (index % (C.BoardSize / 2)) * 2 + row % 2;
            return new Position(row, col);
        }
    }
    //===============================================================================
    internal static class Aux
    {
        public static T Max<T>(params T[] args) where T : IComparable<T>
        {
            T max = args[0];
            foreach (T arg in args)
            {
                if (arg.CompareTo(max) > 0)
                    max = arg;
            }
            return max;
        }
        public static T Min<T>(params T[] args) where T : IComparable<T>
        {
            T min = args[0];
            foreach (T arg in args)
            {
                if (arg.CompareTo(min) < 0)
                    min = arg;
            }
            return min;
        }
        public static void Initialize(out Piece[] board)
        {
            board = new Piece[C.ArrayLength];
            for (uint i = 0; i < C.ArrayLength / 2 - C.BoardSize / 2; ++i)
                board[i] = Piece.White;
            for (uint i = C.ArrayLength / 2 + C.BoardSize / 2; i < C.ArrayLength; ++i)
                board[i] = Piece.Black;
        }
        public static Piece[] Copy(Piece[] original)
        {
            int length = original.Length;
            Piece[] copy = new Piece[length];
            Array.Copy(original, copy, length);
            return copy;
        }
        [Conditional("CONSOLE")]
        public static void Print(Piece[] data) // NOT CONVERTED
        {
            Console.WriteLine("----------------");
            for (uint row = C.BoardSize-1; row >= 0; --row)
            {
                for (uint col = 0; col < C.BoardSize; ++col)
                {
                    Piece p = data[C.BoardSize * row + col];
                    if ((p & Piece.White) == Piece.White)
                    {
                        if ((p & Piece.King) == Piece.King)
                            Console.Write("E ");
                        else
                            Console.Write("e ");
                    }
                    else if ((p & Piece.Black) == Piece.Black)
                    {
                        if ((p & Piece.King) == Piece.King)
                            Console.Write("O ");
                        else
                            Console.Write("o ");
                    }
                    else
                        Console.Write("- ");

                }
                Console.WriteLine();
            }
            Console.WriteLine("----------------");
        }
    }
    //===============================================================================
    /// <summary>
    /// Moves right pieces to right positions if possible
    /// </summary>
    //===============================================================================
    internal interface IPiececontroller
    {
        /// <summary>
        /// Never creates a Board.
        /// </summary>
        Piece[] Board { get; set; }
        /// <summary>
        /// Either all black or all white. 
        /// </summary>
        List<Position> Positions { get; }
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanJumpRight(Position pos);
        /// <summary>
        /// Whatever is at 'pos' always jumps to the right, possible multiple jumps
        /// </summary>
        void JumpRight(Position pos);
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanJumpLeft(Position pos);
        /// <summary>
        /// Whatever is at 'pos' always jumps to the left, possible multiple jumps
        /// </summary>
        void JumpLeft(Position pos);
        /// <summary>
        /// Might return mutiple directions
        /// </summary>
        Direction CanJumpKing(Position pos);
        /// <summary>
        /// Undefined behavior if invalide (or multiple) direction
        /// </summary>
        void JumpKing(Direction dirn, Position pos);
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanMoveRight(Position pos);
        /// <summary>
        /// Whatever is at 'pos' always moves to the right
        /// </summary>
        void MoveRight(Position pos);
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanMoveLeft(Position pos);
        /// <summary>
        /// Whatever is at 'pos' always moves to the left
        /// </summary>
        void MoveLeft(Position pos);
        /// <summary>
        /// Might return mutiple directions
        /// </summary>
        Direction CanMoveKing(Position pos);
        /// <summary>
        /// Undefined behavior if invalide (or multiple) direction
        /// </summary>
        void MoveKing(Direction dirn, Position pos);
    }
    //===============================================================================
    internal class WhitePiececontroller : IPiececontroller
    {
        public WhitePiececontroller(Piece[] board)
        {
            m_Board = board;
        }
        public Piece[] Board
        {
            get { return m_Board; }
            set { m_Board = value; }
        }
        public List<Position> Positions
        {
            get
            {
                var allwhite = new List<Position>();
                for (uint i = 0; i < m_Board.Length; ++i)
                    if ((m_Board[i] & Piece.White) == Piece.White)
                        allwhite.Add(i);
                return allwhite;
            }
        }
        public bool CanJumpRight(Position pos)
        {
            if (pos.Col > C.BoardSize - 3 || pos.Row > C.BoardSize - 3)
                return false;
            if ((m_Board[pos.Offset(1, 1)] & Piece.Black) == Piece.Black &&
                m_Board[pos.Offset(2, 2)] == Piece.Empty)
                return true;
            return false;
        }
        public void JumpRight(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            m_Board[pos.Offset(1, 1)] = Piece.Empty;
            Position newpos = pos.Offset(2, 2);
            if (newpos.Row == C.BoardSize - 1)
                m_Board[newpos] = (Piece.White | Piece.King);
            else
            {
                m_Board[newpos] = mypiece;

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(newpos.Offset(2, 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(newpos.Offset(2, -2)) + 1;

                if (numrightjump != 0 || numrightjump != 0)
                {   // Multiple jumps
                    if (numrightjump > numleftjump)
                        JumpRight(newpos);
                    else
                        JumpLeft(newpos);
                }
            }
        }
        public bool CanJumpLeft(Position pos)
        {
            if (pos.Col < 2 || pos.Row > C.BoardSize - 3)
                return false;
            if ((m_Board[pos.Offset(1, -1)] & Piece.Black) == Piece.Black &&
                m_Board[pos.Offset(2, -2)] == Piece.Empty)
                return true;
            return false;
        }
        public void JumpLeft(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            m_Board[pos.Offset(1, -1)] = Piece.Empty;
            Position newpos = pos.Offset(2, -2);
            if (newpos.Row == C.BoardSize - 1)
                m_Board[newpos] = (Piece.White | Piece.King);
            else
            {
                m_Board[newpos] = mypiece;

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(newpos.Offset(2, 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(newpos.Offset(2, -2)) + 1;

                if (numrightjump != 0 || numleftjump != 0)
                {   // Multiple jumps
                    if (numrightjump > numleftjump)
                        JumpRight(newpos);
                    else
                        JumpLeft(newpos);
                }
            }
        }
        public Direction CanJumpKing(Position pos)
        {
            Direction where = Direction.None;
            if (pos.Row < C.BoardSize - 2)
            {
                if (pos.Col < C.BoardSize - 2 &&
                    (m_Board[pos.Offset(1, 1)] & Piece.Black) == Piece.Black &&
                    m_Board[pos.Offset(2, 2)] == Piece.Empty)
                    where |= Direction.RightUp;
                if (pos.Col > 1 &&
                    (m_Board[pos.Offset(1, -1)] & Piece.Black) == Piece.Black &&
                    (m_Board[pos.Offset(2, -2)] == Piece.Empty))
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < C.BoardSize - 2 &&
                    (m_Board[pos.Offset(-1, 1)] & Piece.Black) == Piece.Black &&
                    (m_Board[pos.Offset(-2, 2)] == Piece.Empty))
                    where |= Direction.RightDown;
                if (pos.Col > 1 &&
                    (m_Board[pos.Offset(-1, -1)] & Piece.Black) == Piece.Black &&
                    (m_Board[pos.Offset(-2, -2)] == Piece.Empty))
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void JumpKing(Direction dirn, Position pos)
        {
            Position newpos;
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;

            if (dirn == Direction.RightUp)
            {
                m_Board[pos.Offset(1, 1)] = Piece.Empty;
                newpos = pos.Offset(2, 2);
            }
            else if (dirn == Direction.LeftUp)
            {
                m_Board[pos.Offset(1, -1)] = Piece.Empty;
                newpos = pos.Offset(2, -2);
            }
            else if (dirn == Direction.RightDown)
            {
                m_Board[pos.Offset(-1, 1)] = Piece.Empty;
                newpos = pos.Offset(-2, 2);
            }
            else if (dirn == Direction.LeftDown)
            {
                m_Board[pos.Offset(-1, -1)] = Piece.Empty;
                newpos = pos.Offset(-2, -2);
            }
            else
                return;
            m_Board[newpos] = mypiece;

            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction newdirn = CanJumpKing(newpos);
            if ((newdirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(newpos.Offset(2, 2), Direction.LeftDown) + 1;
            if ((newdirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(newpos.Offset(2, -2), Direction.RightDown) + 1;
            if ((newdirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(newpos.Offset(-2, 2), Direction.LeftUp) + 1;
            if ((newdirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(newpos.Offset(-2, -2), Direction.RightUp) + 1;

            if (rightDown != 0 || rightUp != 0 || leftDown != 0 || leftUp != 0)
            {
                if (rightUp >= rightDown && rightUp >= leftUp && rightUp >= leftDown)
                    JumpKing(Direction.RightUp, newpos);
                else if (rightDown >= rightUp && rightDown >= leftUp && rightDown >= leftDown)
                    JumpKing(Direction.RightDown, newpos);
                else if (leftDown >= rightDown && leftDown >= leftUp && leftDown >= rightUp)
                    JumpKing(Direction.LeftDown, newpos);
                else
                    JumpKing(Direction.LeftUp, newpos);
            }
        }
        public bool CanMoveRight(Position pos)
        {
            if (pos.Col > C.BoardSize - 2 || pos.Row > C.BoardSize - 2)
                return false;
            if (m_Board[pos.Offset(1, 1)] == Piece.Empty)
                return true;
            return false;
        }
        public void MoveRight(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            if (pos.Row + 1 == C.BoardSize - 1)
                m_Board[pos.Offset(1, 1)] = (Piece.White | Piece.King);
            else
                m_Board[pos.Offset(1, 1)] = mypiece;
        }
        public bool CanMoveLeft(Position pos)
        {
            if (pos.Col < 1 || pos.Row > C.BoardSize - 2)
                return false;
            if (m_Board[pos.Offset(1, -1)] == Piece.Empty)
                return true;
            return false;
        }
        public void MoveLeft(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            if (pos.Row + 1 == C.BoardSize - 1)
                m_Board[pos.Offset(1, -1)] = (Piece.White | Piece.King);
            else
                m_Board[pos.Offset(1, -1)] = mypiece;
        }
        public Direction CanMoveKing(Position pos)
        {
            Direction where = Direction.None;
            if (pos.Row < C.BoardSize - 1)
            {
                if (pos.Col < C.BoardSize - 1 &&
                    m_Board[pos.Offset(1, 1)] == Piece.Empty)
                    where |= Direction.RightUp;
                if (pos.Col > 0 &&
                    m_Board[pos.Offset(1, -1)] == Piece.Empty)
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < C.BoardSize - 1 &&
                    m_Board[pos.Offset(-1, 1)] == Piece.Empty)
                    where |= Direction.RightDown;
                if (pos.Col > 0 &&
                    m_Board[pos.Offset(-1, -1)] == Piece.Empty)
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void MoveKing(Direction dirn, Position pos)
        {
            if (dirn == Direction.None)
                return;
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            if ((dirn & Direction.RightUp) == Direction.RightUp)
                m_Board[pos.Offset(1, 1)] = mypiece;
            else if ((dirn & Direction.LeftUp) == Direction.LeftUp)
                m_Board[pos.Offset(1, -1)] = mypiece;
            else if ((dirn & Direction.RightDown) == Direction.RightDown)
                m_Board[pos.Offset(-1, 1)] = mypiece;
            else if ((dirn & Direction.LeftDown) == Direction.LeftDown)
                m_Board[pos.Offset(-1, -1)] = mypiece;
        }
        /// <summary>
        /// Returns max number of possible successive jumps from 'pos'
        /// </summary>
        private int MaxJump(Position pos)
        {
            int rightjump=0, leftjump=0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(pos.Offset(2, 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(pos.Offset(2, -2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        /// <summary>
        /// Returns max number of possible successive jumps for a King at 'pos'.
        /// 'nothere' is the direction where it cannot jump from 'pos'.
        /// </summary>
        private int MaxJumpKing(Position pos, Direction nothere)
        {
            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction.RightUp && (dirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(pos.Offset(2, 2), Direction.LeftDown) + 1;
            if (nothere != Direction.LeftUp && (dirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(pos.Offset(2, -2), Direction.RightDown) + 1;
            if (nothere != Direction.RightDown && (dirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(pos.Offset(-2, 2), Direction.LeftUp) + 1;
            if (nothere != Direction.LeftDown && (dirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(pos.Offset(-2, -2), Direction.RightUp) + 1;
            return Aux.Max(rightUp, rightDown, leftUp, leftDown);
        }
        Piece[] m_Board;
    }
    //===============================================================================
    internal class BlackPiececontroller : IPiececontroller
    {
        public BlackPiececontroller(BoardBuilder b)
        {
            m_Builder = b;
        }
        public BoardBuilder Builder
        {
            get { return m_Builder; }
            set { if (value != null) m_Builder = value; }
        }
        public bool CanJumpRight(Position pos)
        {
            if (pos.Col > C.BoardSize - 3 || pos.Row < 2)
                return false;
            if ((m_Builder.GetAt(pos.Row - 1, pos.Col + 1) & Piece.White) == Piece.White &&
                m_Builder.GetAt(pos.Row - 2, pos.Col + 2) == Piece.Empty)
                return true;
            return false;
        }
        public void JumpRight(Position pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            m_Builder.SetAt(pos.Row - 1, pos.Col + 1, Piece.Empty);
            Position newpos = new Position(pos.Row - 2, pos.Col + 2);
            if (newpos.Row == 0)
                m_Builder.SetAt(newpos, (Piece.Black | Piece.King));
            else
            {
                m_Builder.SetAt(newpos, mypiece);

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(new Position(newpos.Row - 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(new Position(newpos.Row - 2, newpos.Col - 2)) + 1;

                if (numrightjump != 0 || numrightjump != 0)
                {   // Multiple jumps
                    if (numrightjump > numleftjump)
                        JumpRight(newpos);
                    else
                        JumpLeft(newpos);
                }
            }
        }
        public bool CanJumpLeft(Position pos)
        {
            if (pos.Col < 2 || pos.Row < 2)
                return false;
            if ((m_Builder.GetAt(pos.Row - 1, pos.Col - 1) & Piece.White) == Piece.White &&
                m_Builder.GetAt(pos.Row - 2, pos.Col - 2) == Piece.Empty)
                return true;
            return false;
        }
        public void JumpLeft(Position pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            m_Builder.SetAt(pos.Row - 1, pos.Col - 1, Piece.Empty);
            Position newpos = new Position(pos.Row - 2, pos.Col - 2);
            if (newpos.Row == 0)
                m_Builder.SetAt(newpos, (Piece.Black | Piece.King));
            else
            {
                m_Builder.SetAt(newpos, mypiece);

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(new Position(newpos.Row - 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(new Position(newpos.Row - 2, newpos.Col - 2)) + 1;

                if (numrightjump != 0 || numleftjump != 0)
                {   // Multiple jumps
                    if (numrightjump > numleftjump)
                        JumpRight(newpos);
                    else
                        JumpLeft(newpos);
                }
            }
        }
        public Direction CanJumpKing(Position pos)
        {
            Direction where = Direction.None;
            if (pos.Row < C.BoardSize - 2)
            {
                if (pos.Col < C.BoardSize - 2 &&
                    (m_Builder.GetAt(pos.Row + 1, pos.Col + 1) & Piece.White) == Piece.White &&
                    (m_Builder.GetAt(pos.Row + 2, pos.Col + 2) == Piece.Empty))
                    where |= Direction.RightUp;
                if (pos.Col > 1 &&
                    (m_Builder.GetAt(pos.Row + 1, pos.Col - 1) & Piece.White) == Piece.White &&
                    (m_Builder.GetAt(pos.Row + 2, pos.Col - 2) == Piece.Empty))
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < C.BoardSize - 2 &&
                    (m_Builder.GetAt(pos.Row - 1, pos.Col + 1) & Piece.White) == Piece.White &&
                    (m_Builder.GetAt(pos.Row - 2, pos.Col + 2) == Piece.Empty))
                    where |= Direction.RightDown;
                if (pos.Col > 1 &&
                    (m_Builder.GetAt(pos.Row - 1, pos.Col - 1) & Piece.White) == Piece.White &&
                    (m_Builder.GetAt(pos.Row - 2, pos.Col - 2) == Piece.Empty))
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void JumpKing(Direction dirn, Position pos)   // Same as for White
        {
            Position newpos;
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);

            if (dirn == Direction.RightUp)
            {
                m_Builder.SetAt(pos.Row + 1, pos.Col + 1, Piece.Empty);
                newpos = new Position(pos.Row + 2, pos.Col + 2);
            }
            else if (dirn == Direction.LeftUp)
            {
                m_Builder.SetAt(pos.Row + 1, pos.Col - 1, Piece.Empty);
                newpos = new Position(pos.Row + 2, pos.Col - 2);
            }
            else if (dirn == Direction.RightDown)
            {
                m_Builder.SetAt(pos.Row - 1, pos.Col + 1, Piece.Empty);
                newpos = new Position(pos.Row - 2, pos.Col + 2);
            }
            else if (dirn == Direction.LeftDown)
            {
                m_Builder.SetAt(pos.Row - 1, pos.Col - 1, Piece.Empty);
                newpos = new Position(pos.Row - 2, pos.Col - 2);
            }
            else
                return;
            m_Builder.SetAt(newpos, mypiece);


            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction newdirn = CanJumpKing(newpos);
            if ((newdirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(new Position(newpos.Row + 2, newpos.Col + 2), Direction.LeftDown) + 1;
            if ((newdirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(new Position(newpos.Row + 2, newpos.Col - 2), Direction.RightDown) + 1;
            if ((newdirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(new Position(newpos.Row - 2, newpos.Col + 2), Direction.LeftUp) + 1;
            if ((newdirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(new Position(newpos.Row - 2, newpos.Col - 2), Direction.RightUp) + 1;

            if (rightDown != 0 || rightUp != 0 || leftDown != 0 || leftUp != 0)
            {
                if (rightUp >= rightDown && rightUp >= leftUp && rightUp >= leftDown)
                    JumpKing(Direction.RightUp, newpos);
                else if (rightDown >= rightUp && rightDown >= leftUp && rightDown >= leftDown)
                    JumpKing(Direction.RightDown, newpos);
                else if (leftDown >= rightDown && leftDown >= leftUp && leftDown >= rightUp)
                    JumpKing(Direction.LeftDown, newpos);
                else
                    JumpKing(Direction.LeftUp, newpos);
            }
        }
        public bool CanMoveRight(Position pos)
        {
            if (pos.Col > C.BoardSize - 2 || pos.Row < 1)
                return false;
            if (m_Builder.GetAt(pos.Row - 1, pos.Col + 1) == Piece.Empty)
                return true;
            return false;
        }
        public void MoveRight(Position pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            if (pos.Row - 1 == 0)
                m_Builder.SetAt(pos.Row - 1, pos.Col + 1, (Piece.Black | Piece.King));
            else
                m_Builder.SetAt(pos.Row - 1, pos.Col + 1, mypiece);
        }
        public bool CanMoveLeft(Position pos)
        {
            if (pos.Col < 1 || pos.Row < 1)
                return false;
            if (m_Builder.GetAt(pos.Row - 1, pos.Col - 1) == Piece.Empty)
                return true;
            return false;
        }
        public void MoveLeft(Position pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            if (pos.Row - 1 == 0)
                m_Builder.SetAt(pos.Row - 1, pos.Col - 1, (Piece.Black | Piece.King));
            else
                m_Builder.SetAt(pos.Row - 1, pos.Col - 1, mypiece);
        }
        public Direction CanMoveKing(Position pos)   // same as White
        {
            Direction where = Direction.None;
            if (pos.Row < C.BoardSize - 1)
            {
                if (pos.Col < C.BoardSize - 1 &&
                    m_Builder.GetAt(pos.Row + 1, pos.Col + 1) == Piece.Empty)
                    where |= Direction.RightUp;
                if (pos.Col > 0 &&
                    m_Builder.GetAt(pos.Row + 1, pos.Col - 1) == Piece.Empty)
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < C.BoardSize - 1 &&
                    m_Builder.GetAt(pos.Row - 1, pos.Col + 1) == Piece.Empty)
                    where |= Direction.RightDown;
                if (pos.Col > 0 &&
                    m_Builder.GetAt(pos.Row - 1, pos.Col - 1) == Piece.Empty)
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void MoveKing(Direction dirn, Position pos)   // same as White
        {
            if (dirn == Direction.None)
                return;
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            if ((dirn & Direction.RightUp) == Direction.RightUp)
                m_Builder.SetAt(pos.Row + 1, pos.Col + 1, mypiece);
            else if ((dirn & Direction.LeftUp) == Direction.LeftUp)
                m_Builder.SetAt(pos.Row + 1, pos.Col - 1, mypiece);
            else if ((dirn & Direction.RightDown) == Direction.RightDown)
                m_Builder.SetAt(pos.Row - 1, pos.Col + 1, mypiece);
            else if ((dirn & Direction.LeftDown) == Direction.LeftDown)
                m_Builder.SetAt(pos.Row - 1, pos.Col - 1, mypiece);
        }
        public bool IsValid(bool king, Position pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            if ((mypiece & Piece.Black) != Piece.Black)
                return false;
            if (king && (mypiece & Piece.King) == Piece.King)
                return true;
            if (!king && (mypiece & Piece.King) == 0)
                return true;
            return false;
        }
        /// <summary>
        /// Returns max number of possible successive jumps from 'pos'
        /// </summary>
        private int MaxJump(Position pos)
        {
            int rightjump=0, leftjump=0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(new Position(pos.Row - 2, pos.Col + 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(new Position(pos.Row - 2, pos.Col - 2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        /// <summary>
        /// Returns max number of possible successive jumps for a King at 'pos'.
        /// 'nothere' is the direction where it cannot jump from 'pos'.
        /// </summary>
        private int MaxJumpKing(Position pos, Direction nothere) // same as for White
        {
            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction.RightUp && (dirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(new Position(pos.Row + 2, pos.Col + 2), Direction.LeftDown) + 1;
            if (nothere != Direction.LeftUp && (dirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(new Position(pos.Row + 2, pos.Col - 2), Direction.RightDown) + 1;
            if (nothere != Direction.RightDown && (dirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(new Position(pos.Row - 2, pos.Col + 2), Direction.LeftUp) + 1;
            if (nothere != Direction.LeftDown && (dirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(new Position(pos.Row - 2, pos.Col - 2), Direction.RightUp) + 1;
            return Aux.Max(rightUp, rightDown, leftUp, leftDown);
        }
        BoardBuilder m_Builder;
    }
    //===============================================================================
    /// <summary>
    /// Creates an array of child states from the current game state.
    /// To be created only once, then takes different fields.
    /// </summary>
    //===============================================================================
    internal class ChildGetter
    {
        private enum MoveType : byte
        {
            Unset, Move, Jump
        }
        public ChildGetter()
        {
            m_Move = MoveType.Unset;        // configured by CurrentState.Set
            m_Pc = null;                    // initialized by SetColor
            m_Build = new BoardBuilder();   // configured by CurrentState.Set
            m_Righties = null;              // initialized by CurrentState.Set
            m_Lefties = null;               // initialized by CurrentState.Set
            m_Kings = null;                 // initialized by CurrentState.Set
            m_Kingdirns = null;             // initialized by CurrentState.Set
        }
        /// <summary>
        /// Whose turn it is now
        /// </summary>
        public void SetColor(bool white)
        {
            if (white) m_Pc = new WhitePiececontroller(m_Build);
            else m_Pc = new BlackPiececontroller(m_Build);
        }
        /// <summary>
        /// Updates all internal states except color. Color should be updated prior to this.
        /// </summary>
        public Piece[] CurrentState
        {
            get { return m_Build.Field; }
            set
            {
                m_Build.Field = value;

                if (m_Righties == null)
                    m_Righties = new List<Position>();
                else
                    m_Righties.Clear();

                if (m_Lefties == null)
                    m_Lefties = new List<Position>();
                else
                    m_Lefties.Clear();

                if (m_Kings == null)
                    m_Kings = new List<Position>();
                else
                    m_Kings.Clear();

                if (m_Kingdirns == null)
                    m_Kingdirns = new List<Direction>();
                else
                    m_Kingdirns.Clear();

                UpdateMembers();
            }
        }
        /// <summary>
        /// All-in-one initialization, recommended instead of SetColor and CurrentState
        /// </summary>
        void Configure(bool whiteColor, Piece[] newstate)
        {
            SetColor(whiteColor);
            CurrentState = newstate;
        }
        /// <summary>
        /// All members must be initialized and configured (by CurrentState and SetColor()).
        /// </summary>
        public List<Piece[]> Childs
        {
            get
            {
                var childs = new List<Piece[]>();
                foreach(Position p in m_Righties)
                {
                    m_Build.ResetCounter();
                    if (m_Move == MoveType.Jump)
                        m_Pc.JumpRight(p);
                    else
                        m_Pc.MoveRight(p);
                    childs.Add(m_Build.Copy);
                    m_Build.Reset();
                }
                foreach(Position p in m_Lefties)
                {
                    m_Build.ResetCounter();
                    if (m_Move == MoveType.Jump)
                        m_Pc.JumpLeft(p);
                    else
                        m_Pc.MoveLeft(p);
                    childs.Add(m_Build.Copy);
                    m_Build.Reset();
                }
                for (int i = 0; i < m_Kings.Count; ++i)
                {
                    Direction dirn = m_Kingdirns[i];
                    Position p = m_Kings[i];
                    if (m_Move == MoveType.Jump)
                    {
                        if ((dirn & Direction.LeftDown) == Direction.LeftDown)
                        {
                            m_Build.ResetCounter();
                            m_Pc.JumpKing(Direction.LeftDown, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                        if ((dirn & Direction.LeftUp) == Direction.LeftUp)
                        {
                            m_Build.ResetCounter();
                            m_Pc.JumpKing(Direction.LeftUp, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                        if ((dirn & Direction.RightDown) == Direction.RightDown)
                        {
                            m_Build.ResetCounter();
                            m_Pc.JumpKing(Direction.RightDown, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                        if ((dirn & Direction.RightUp) == Direction.RightUp)
                        {
                            m_Build.ResetCounter();
                            m_Pc.JumpKing(Direction.RightUp, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                    }
                    else // if (m_Move == MoveType.Move)
                    {
                        if ((dirn & Direction.LeftDown) == Direction.LeftDown)
                        {
                            m_Build.ResetCounter();
                            m_Pc.MoveKing(Direction.LeftDown, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                        if ((dirn & Direction.LeftUp) == Direction.LeftUp)
                        {
                            m_Build.ResetCounter();
                            m_Pc.MoveKing(Direction.LeftUp, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                        if ((dirn & Direction.RightDown) == Direction.RightDown)
                        {
                            m_Build.ResetCounter();
                            m_Pc.MoveKing(Direction.RightDown, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                        if ((dirn & Direction.RightUp) == Direction.RightUp)
                        {
                            m_Build.ResetCounter();
                            m_Pc.MoveKing(Direction.RightUp, p);
                            childs.Add(m_Build.Copy);
                            m_Build.Reset();
                        }
                    }
                }
                return childs;
            }
        }
        /// <summary>
        /// Returns value from 0.0 (loss) to 1.0 (win), 0.5 is a draw
        /// </summary>
        public double HeuristicValue(bool white_is_max_side)
        {
            int numWhite = m_Build.NumberWhites;
            int numBlack = m_Build.NumberBlacks;
            int numerator = (white_is_max_side) ? numWhite : numBlack;
            return (double)numerator / (numWhite + numBlack);
        }
        public bool Jumping
        {
            get { return m_Move == MoveType.Jump; }
        }
        public bool Moving
        {
            get { return m_Move == MoveType.Move; }
        }
        public IPiececontroller Controller
        {
            get { return m_Pc; }
        }
        /// <summary>
        /// Either only jumpers ('true') or only movers ('false'). Doesn't clear the lists.
        /// </summary>
        private void UpdateMembers()
        {
            Position[] allPositions;
            if (m_Pc is WhitePiececontroller)
                allPositions = m_Build.WhitePositions;
            else if (m_Pc is BlackPiececontroller)
                allPositions = m_Build.BlackPositions;
            else
                throw new InvalidOperationException();
            
            bool jump = false;
            foreach (Position p in allPositions)
            {
                if ((m_Build.GetAt(p) & Piece.King) == Piece.King)
                {
                    Direction where = m_Pc.CanJumpKing(p);
                    if (where != Direction.None)
                    {
                        if (!jump)
                        {
                            m_Kings.Clear();
                            m_Kingdirns.Clear();
                            m_Righties.Clear();
                            m_Lefties.Clear();
                            jump = true;
                        }
                        m_Kings.Add(p);
                        m_Kingdirns.Add(where);
                    }
                    else if (!jump && (where = m_Pc.CanMoveKing(p)) != Direction.None) 
                    {
                        m_Kings.Add(p);
                        m_Kingdirns.Add(where);
                    }
                }
                else // not a king
                {
                    if (m_Pc.CanJumpRight(p))
                    {
                        if (!jump)
                        {
                            m_Righties.Clear();
                            m_Lefties.Clear();
                            m_Kings.Clear();
                            m_Kingdirns.Clear();
                            jump = true;
                        }
                        m_Righties.Add(p);
                    }
                    if (m_Pc.CanJumpLeft(p))
                    {
                        if (!jump)
                        {
                            m_Lefties.Clear();
                            m_Righties.Clear();
                            m_Kings.Clear();
                            m_Kingdirns.Clear();
                            jump = true;
                        }
                        m_Lefties.Add(p);
                    }
                    else if (!jump)
                    {
                        if (m_Pc.CanMoveRight(p)) m_Righties.Add(p);
                        if (m_Pc.CanMoveLeft(p)) m_Lefties.Add(p);
                    }
                }
            }
            m_Move = (jump) ? MoveType.Jump : MoveType.Move;
        }
        MoveType m_Move;
        IPiececontroller m_Pc;
        List<Position> m_Righties;           // to move or jump to the right
        List<Position> m_Lefties;            // to move or jump to the left
        List<Position> m_Kings;              // Kings that can move or jump
        List<Direction> m_Kingdirns;    // where the kings can go
        BoardBuilder m_Build;
    }
    //===============================================================================
    /// <summary>
    /// The game
    /// </summary>
    //===============================================================================
    public class Gameplay
    {
        public Gameplay(bool AI_is_white) : this(AI_is_white, 8, 9)
        { }
        public Gameplay(bool AI_is_white, uint boardSize, int depth)
        {
            C.BoardSize = boardSize;
            m_Depth = depth;
            m_WhiteAI = AI_is_white;
            m_Cg = new ChildGetter();
            BoardBuilder.Initialize(out m_Board);
        }
        /// <summary>
        /// Current game state.
        /// </summary>
        public Piece[] Board
        {
            get { return m_Board; }
        }
        /// <summary>
        /// Updates board if possible, otherwise returns false.
        /// </summary>
        public bool AITurn()
        {
            m_Cg.SetColor((m_WhiteAI) ? C.White : C.Black);
            m_Cg.CurrentState = m_Board;

            var children = m_Cg.Childs;
            if (children.Count == 0)
                return false;

            var childValues = new double[children.Count];
            int bestChildInd = 0;
            for (int i = 0; i < children.Count; ++i)
            {
                childValues[i] = AlphaBetaPruning(children[i], m_Depth);
                if (childValues[bestChildInd] < childValues[i])
                    bestChildInd = i;
            }
            m_Board = children[bestChildInd];
            return true;
        }
        /// <summary>
        /// Returns 'false' if invalid arguments (impossible to perform the move).
        /// </summary>
        public bool PlayerTurn(Position pos, Direction dirn) // Ugly method
        {   
            m_Cg.SetColor((m_WhiteAI) ? C.Black : C.White);
            m_Cg.CurrentState = m_Board;
            
            if ((m_Board[pos] & Piece.King) == Piece.King)
            {
                Direction whereCanGo = (m_Cg.Moving) ? m_Cg.Controller.CanMoveKing(pos) : m_Cg.Controller.CanJumpKing(pos);
                if ((whereCanGo & dirn) == dirn)
                {
                    if (m_Cg.Moving) m_Cg.Controller.MoveKing(dirn, pos);
                    else m_Cg.Controller.JumpKing(dirn, pos);
                }
                else
                    return false;
            }
            else // not a king
            {
                if (m_Cg.Jumping)
                {
                    if ((dirn & (Direction.RightUp | Direction.RightDown)) == dirn && m_Cg.Controller.CanJumpRight(pos))
                        m_Cg.Controller.JumpRight(pos);
                    else if ((dirn & (Direction.LeftUp | Direction.LeftDown)) == dirn && m_Cg.Controller.CanJumpLeft(pos))
                        m_Cg.Controller.JumpLeft(pos);
                    else
                        return false;
                }
                else // moving
                {
                    if ((dirn & (Direction.RightUp | Direction.RightDown)) == dirn && m_Cg.Controller.CanMoveRight(pos))
                        m_Cg.Controller.MoveRight(pos);
                    else if ((dirn & (Direction.LeftUp | Direction.LeftDown)) == dirn && m_Cg.Controller.CanMoveLeft(pos))
                        m_Cg.Controller.MoveLeft(pos);
                    else
                        return false;
                }
            }
            return true;
        }
        private double AlphaBetaPruning(Piece[] node, int depth, double alpha = 0.0, double beta = 1.0, bool aiturn = false)
        {
            if (m_WhiteAI)
                m_Cg.SetColor((aiturn) ? C.White : C.Black);
            else
                m_Cg.SetColor((aiturn) ? C.Black : C.White);
            m_Cg.CurrentState = node;

            double val = m_Cg.HeuristicValue(m_WhiteAI);
            if ((m_Depth != -1 && depth == 0) || val == 0.0 || val == 1.0)
                return val;

            List<Piece[]> children = m_Cg.Childs;
            if (aiturn)
            {
                val = 0.0;
                foreach(Piece[] child in children)
                {
                    val = Aux.Max(val, AlphaBetaPruning(child, depth - 1, alpha, beta, false));
                    alpha = Aux.Max(alpha, val);
                    if (beta <= alpha)
                        break;
                }
            }
            else
            {
                val = 1.0;
                foreach(Piece[] child in children)
                {
                    val = Aux.Min(val, AlphaBetaPruning(child, depth - 1, alpha, beta, true));
                    beta = Aux.Min(beta, val);
                    if (beta <= alpha)
                        break;
                }
            }
            return val;
        }
        int         m_Depth;
        bool        m_WhiteAI;
        ChildGetter m_Cg;
        Piece[]     m_Board;
    }
}