﻿// Rules: https://en.wikipedia.org/wiki/English_draughts
// For console version the macro CONSOLE must be defined
using System;
using System.Diagnostics;
using System.Linq;
using System.Collections.Generic;

namespace Checkers
{
    [Flags]
    public enum Piece : byte
    {
        Empty = 0x0,
        White = 0x1,
        Black = 0x2,
        King = 0x4
    }
    [Flags]
    public enum Direction : byte
    {
        None = 0x0,
        RightUp = 0x1,
        LeftUp = 0x2,
        RightDown = 0x4,
        LeftDown = 0x8
    }
    //===============================================================================
    /// <summary>
    /// Constants and global variables.
    /// </summary>
    internal static class C
    {
        /// <summary> Substitution for #define WHITE true </summary>
        public const bool White = true;
        /// <summary> Substitution for #define BLACK false </summary>
        public const bool Black = false;
        /// <summary> 
        /// Size of the board's side, always even. Updates ArrayLength when set.
        /// </summary>
        public static int BoardSize
        {
            get { return m_Size; }
            set
            {   // enforced to be even
                m_Size = value + value % 2;
                m_Length = m_Size * m_Size / 2;
            }
        }
        public static int ArrayLength
        {
            get { return m_Length; }
        }
        private static int m_Size;
        private static int m_Length;
    }
    /// <summary>
    /// Immutable, can be converted from/to array index
    /// </summary>
    internal struct Position
    {
        public Position(int row, int col) : this()
        {
            Row = row;
            Col = col;
        }
        public int Row { get; }
        public int Col { get; }
        /// <summary> Returns new position with given offset from 'this'. </summary>
        public Position Offset(int rowoffset, int coloffset)
        {
            int row = Row + rowoffset;
            int col = Col + coloffset;
            return new Position(row, col);
        }
        /// <summary> Converts position to array index </summary>
        public static implicit operator int(Position p)
        {
            return p.Row * (C.BoardSize / 2) + p.Col / 2;
        }
        /// <summary> Converts array index to position </summary>
        public static explicit operator Position(int index)
        {
            int row = index / (C.BoardSize / 2);
            int col = (index % (C.BoardSize / 2)) * 2 + row % 2;
            return new Position(row, col);
        }
        /// <summary>
        /// Comparing positions by value
        /// </summary>
        public static bool operator==(Position pos1, Position pos2)
        {
            return (pos1.Row == pos2.Row) && (pos1.Col == pos2.Col);
        }
        public static bool operator !=(Position pos1, Position pos2)
        {
            return (pos1.Row != pos2.Row) && (pos1.Col != pos2.Col);
        }
    }
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
            for (int i = 0; i < C.ArrayLength / 2 - C.BoardSize / 2; ++i)
                board[i] = Piece.White;
            for (int i = C.ArrayLength / 2 + C.BoardSize / 2; i < C.ArrayLength; ++i)
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
        public static void Print(Gameplay game)
        {
            Console.WriteLine("----------------");
            for (int row = C.BoardSize-1; row >= 0; --row)
            {
                for (int col = 0; col < C.BoardSize; ++col)
                {
                    Piece p = game[row, col];
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
        [Conditional("DEBUG")]
        public static void Print(Piece[] board)
        {
            Console.WriteLine("----------------");
            for (int row = C.BoardSize - 1; row >= 0; --row)
            {
                for (int col = 0; col < C.BoardSize; ++col)
                {
                    Piece p;
                    if ((row + col) % 2 == 1)
                        p = Piece.Empty;
                    else
                        p = board[new Position(row, col)];

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
        /// <summary> Current board to perform operations on </summary>
        Piece[] Board { get; set; }
        /// <summary> Positions of all pieces of corresponding color </summary>
        List<Position> Positions { get; }
        /// <summary> Doesn't check what's at 'pos' </summary>
        bool CanJumpRight(Position pos);
        /// <summary> 
        /// Whatever is at 'pos' always jumps to the right, possible multiple jumps
        /// </summary>
        void JumpRight(Position pos);
        /// <summary> Doesn't check what's at 'pos' </summary>
        bool CanJumpLeft(Position pos);
        /// <summary>
        /// Whatever is at 'pos' always jumps to the left, possible multiple jumps
        /// </summary>
        void JumpLeft(Position pos);
        /// <summary> Might return mutiple directions </summary>
        Direction CanJumpKing(Position pos);
        /// <summary> Undefined behavior if invalide (or multiple) direction </summary>
        void JumpKing(Direction dirn, Position pos);
        /// <summary> Doesn't check what's at 'pos' </summary>
        bool CanMoveRight(Position pos);
        /// <summary> Whatever is at 'pos' always moves to the right </summary>
        void MoveRight(Position pos);
        /// <summary> Doesn't check what's at 'pos' </summary>
        bool CanMoveLeft(Position pos);
        /// <summary> Whatever is at 'pos' always moves to the left </summary>
        void MoveLeft(Position pos);
        /// <summary> Might return mutiple directions </summary>
        Direction CanMoveKing(Position pos);
        /// <summary> Undefined behavior if invalide (or multiple) direction </summary>
        void MoveKing(Direction dirn, Position pos);
    }
    //===============================================================================
    internal class WhitePiececontroller : IPiececontroller
    {
        public WhitePiececontroller(Piece[] board)
        {
            m_Board = board;
        }
        public WhitePiececontroller()
        {
            m_Board = null;
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
                for (int i = 0; i < m_Board.Length; ++i)
                    if ((m_Board[i] & Piece.White) == Piece.White)
                        allwhite.Add((Position)i);
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

                if (numrightjump != 0 || numleftjump != 0)
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
                rightUp = MaxJumpKing(newpos.Offset(2, 2), Direction.LeftDown, newpos) + 1;
            if ((newdirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(newpos.Offset(2, -2), Direction.RightDown, newpos) + 1;
            if ((newdirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(newpos.Offset(-2, 2), Direction.LeftUp, newpos) + 1;
            if ((newdirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(newpos.Offset(-2, -2), Direction.RightUp, newpos) + 1;

            if (rightDown != 0 || rightUp != 0 || leftDown != 0 || leftUp != 0)
            {   // Jump where greatest number of jumps
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
        private int MaxJumpKing(Position pos, Direction nothere, Position initpos) // same as for White
        {
            if (pos == initpos)
                return 0;
            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction.RightUp && (dirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(pos.Offset(2, 2), Direction.LeftDown, initpos) + 1;
            if (nothere != Direction.LeftUp && (dirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(pos.Offset(2, -2), Direction.RightDown, initpos) + 1;
            if (nothere != Direction.RightDown && (dirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(pos.Offset(-2, 2), Direction.LeftUp, initpos) + 1;
            if (nothere != Direction.LeftDown && (dirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(pos.Offset(-2, -2), Direction.RightUp, initpos) + 1;
            return Aux.Max(rightUp, rightDown, leftUp, leftDown);
        }
        Piece[] m_Board;
    }
    //===============================================================================
    internal class BlackPiececontroller : IPiececontroller
    {
        public BlackPiececontroller(Piece[] board)
        {
            m_Board = board;
        }
        public BlackPiececontroller()
        {
            m_Board = null;
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
                var allblack = new List<Position>();
                for (int i = 0; i < m_Board.Length; ++i)
                    if ((m_Board[i] & Piece.Black) == Piece.Black)
                        allblack.Add((Position)i);
                return allblack;
            }
        }
        public bool CanJumpRight(Position pos)
        {
            if (pos.Col > C.BoardSize - 3 || pos.Row < 2)
                return false;
            if ((m_Board[pos.Offset(-1, 1)] & Piece.White) == Piece.White &&
                    m_Board[pos.Offset(-2, 2)] == Piece.Empty)
                    return true;
                return false;
        }
        public void JumpRight(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            m_Board[pos.Offset(-1, 1)] = Piece.Empty;
            Position newpos = pos.Offset(-2, 2);
            if (newpos.Row == 0)
                m_Board[newpos] = (Piece.Black | Piece.King);
            else
            {
                m_Board[newpos] = mypiece;

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(newpos.Offset(-2, 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(newpos.Offset(-2, -2)) + 1;

                if (numrightjump != 0 || numleftjump != 0)
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
            if ((m_Board[pos.Offset(-1, -1)] & Piece.White) == Piece.White &&
                m_Board[pos.Offset(-2, -2)] == Piece.Empty)
                return true;
            return false;
        }
        public void JumpLeft(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            m_Board[pos.Offset(-1, -1)] = Piece.Empty;
            Position newpos = pos.Offset(-2, -2);
            if (newpos.Row == 0)
                m_Board[newpos] = (Piece.Black | Piece.King);
            else
            {
                m_Board[newpos] = mypiece;

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(newpos.Offset(-2, 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(newpos.Offset(-2, -2)) + 1;

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
                    (m_Board[pos.Offset(1, 1)] & Piece.White) == Piece.White &&
                    m_Board[pos.Offset(2, 2)] == Piece.Empty)
                    where |= Direction.RightUp;
                if (pos.Col > 1 &&
                    (m_Board[pos.Offset(1, -1)] & Piece.White) == Piece.White &&
                    (m_Board[pos.Offset(2, -2)] == Piece.Empty))
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < C.BoardSize - 2 &&
                    (m_Board[pos.Offset(-1, 1)] & Piece.White) == Piece.White &&
                    (m_Board[pos.Offset(-2, 2)] == Piece.Empty))
                    where |= Direction.RightDown;
                if (pos.Col > 1 &&
                    (m_Board[pos.Offset(-1, -1)] & Piece.White) == Piece.White &&
                    (m_Board[pos.Offset(-2, -2)] == Piece.Empty))
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void JumpKing(Direction dirn, Position pos)   // Same as for White
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
                rightUp = MaxJumpKing(newpos.Offset(2, 2), Direction.LeftDown, newpos) + 1;
            if ((newdirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(newpos.Offset(2, -2), Direction.RightDown, newpos) + 1;
            if ((newdirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(newpos.Offset(-2, 2), Direction.LeftUp, newpos) + 1;
            if ((newdirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(newpos.Offset(-2, -2), Direction.RightUp, newpos) + 1;

            if (rightDown != 0 || rightUp != 0 || leftDown != 0 || leftUp != 0)
            {   // Jump where greatest number of jumps
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
            if (m_Board[pos.Offset(-1, 1)] == Piece.Empty)
                return true;
            return false;
        }
        public void MoveRight(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            if (pos.Row - 1 == 0)
                m_Board[pos.Offset(-1, 1)] = (Piece.Black | Piece.King);
            else
                m_Board[pos.Offset(-1, 1)] = mypiece;
        }
        public bool CanMoveLeft(Position pos)
        {
            if (pos.Col < 1 || pos.Row < 1)
                return false;
            if (m_Board[pos.Offset(-1, -1)] == Piece.Empty)
                return true;
            return false;
        }
        public void MoveLeft(Position pos)
        {
            Piece mypiece = m_Board[pos];
            m_Board[pos] = Piece.Empty;
            if (pos.Row - 1 == 0)
                m_Board[pos.Offset(-1, -1)] = (Piece.Black | Piece.King);
            else
                m_Board[pos.Offset(-1, -1)] = mypiece;
        }
        public Direction CanMoveKing(Position pos)   // same as White
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
        public void MoveKing(Direction dirn, Position pos)   // same as White
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
                rightjump = MaxJump(pos.Offset(-2, 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(pos.Offset(-2, -2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        /// <summary>
        /// Returns max number of possible successive jumps for a King at 'pos'.
        /// 'nothere' is the direction where it cannot jump from 'pos'.
        /// </summary>
        private int MaxJumpKing(Position pos, Direction nothere, Position initpos) // same as for White
        {
            if (pos == initpos)
                return 0;
            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction.RightUp && (dirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(pos.Offset(2, 2), Direction.LeftDown, initpos) + 1;
            if (nothere != Direction.LeftUp && (dirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(pos.Offset(2, -2), Direction.RightDown, initpos) + 1;
            if (nothere != Direction.RightDown && (dirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(pos.Offset(-2, 2), Direction.LeftUp, initpos) + 1;
            if (nothere != Direction.LeftDown && (dirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(pos.Offset(-2, -2), Direction.RightUp, initpos) + 1;
            return Aux.Max(rightUp, rightDown, leftUp, leftDown);
        }
        Piece[] m_Board;
    }
    //===============================================================================
    /// <summary>
    /// Nodes build a game tree in order to use dynamic programming.
    /// Childgetter obtains child nodes from the current game state.
    /// </summary>
    //===============================================================================
    internal class Node
    {
        public Node(Piece[] board)
        {
            Board = board;
            ChildNodes = null;
        }
        public double Value
        { get; set; }
        public Piece[] Board
        { get; }
        public List<Node> ChildNodes
        { get; set; }
    }
    //===============================================================================
    internal class Childgetter
    {
        private enum MoveType : byte
        {
            Unset, Move, Jump
        }
        /// <summary> Initializes but doesn't configure members. </summary>
        public Childgetter()
        {
            m_Move = MoveType.Unset;        
            m_Pc = null;                    
            m_Righties = new List<Position>();
            m_Lefties = new List<Position>();
            m_Kings = new List<Position>();
            m_Kingdirns = new List<Direction>();
            m_State = null;                 
        }
        /// <summary> Configures/refreshes all internal states. </summary>
        public void Configure(bool whitecolor, Piece[] newstate)
        {
            if (whitecolor)
                m_Pc = new WhitePiececontroller(newstate);
            else
                m_Pc = new BlackPiececontroller(newstate);

            m_State = newstate;
            ClearLists();

            List<Position> allPositions = m_Pc.Positions;

            bool jump = false;
            foreach (Position p in allPositions)
            {
                if ((m_State[p] & Piece.King) == Piece.King)
                {
                    Direction where = m_Pc.CanJumpKing(p);
                    if (where != Direction.None)
                    {
                        if (!jump)
                        {
                            ClearLists();
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
                            ClearLists();
                            jump = true;
                        }
                        m_Righties.Add(p);
                    }
                    if (m_Pc.CanJumpLeft(p))
                    {
                        if (!jump)
                        {
                            ClearLists();
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
        /// <summary> All members must be configured. </summary>
        public List<Node> ChildStates
        {
            get
            {
                if (m_Move == MoveType.Unset)
                    throw new InvalidOperationException("Childgetter not configured");

                var childs = new List<Node>();
                foreach(Position p in m_Righties)
                {
                    Piece[] child = Aux.Copy(m_State);
                    m_Pc.Board = child;
                    
                    if (m_Move == MoveType.Jump)
                        m_Pc.JumpRight(p);
                    else
                        m_Pc.MoveRight(p);
                    childs.Add(new Node(child));
                }
                foreach(Position p in m_Lefties)
                {
                    Piece[] child = Aux.Copy(m_State);
                    m_Pc.Board = child;

                    if (m_Move == MoveType.Jump)
                        m_Pc.JumpLeft(p);
                    else
                        m_Pc.MoveLeft(p);
                    childs.Add(new Node(child));
                }
                for (int i = 0; i < m_Kings.Count; ++i)
                {
                    Direction dirn = m_Kingdirns[i];
                    Position p = m_Kings[i];

                    if (m_Move == MoveType.Jump)
                    {
                        if ((dirn & Direction.LeftDown) == Direction.LeftDown)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.JumpKing(Direction.LeftDown, p);
                            childs.Add(new Node(child));
                        }
                        if ((dirn & Direction.LeftUp) == Direction.LeftUp)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.JumpKing(Direction.LeftUp, p);
                            childs.Add(new Node(child));
                        }
                        if ((dirn & Direction.RightDown) == Direction.RightDown)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.JumpKing(Direction.RightDown, p);
                            childs.Add(new Node(child));
                        }
                        if ((dirn & Direction.RightUp) == Direction.RightUp)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.JumpKing(Direction.RightUp, p);
                            childs.Add(new Node(child));
                        }
                    }
                    else // if (m_Move == MoveType.Move)
                    {
                        if ((dirn & Direction.LeftDown) == Direction.LeftDown)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.MoveKing(Direction.LeftDown, p);
                            childs.Add(new Node(child));
                        }
                        if ((dirn & Direction.LeftUp) == Direction.LeftUp)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.MoveKing(Direction.LeftUp, p);
                            childs.Add(new Node(child));
                        }
                        if ((dirn & Direction.RightDown) == Direction.RightDown)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.MoveKing(Direction.RightDown, p);
                            childs.Add(new Node(child));
                        }
                        if ((dirn & Direction.RightUp) == Direction.RightUp)
                        {
                            Piece[] child = Aux.Copy(m_State);
                            m_Pc.Board = child;
                            m_Pc.MoveKing(Direction.RightUp, p);
                            childs.Add(new Node(child));
                        }
                    }
                }
                return childs;
            }
        }
        /// <summary> Returns value from 0.0 (loss) to 1.0 (win), 0.5 is a draw. </summary>
        public double HeuristicValue(bool white_is_max_side)
        {
            int numWhite = 0;
            int numBlack = 0;
            foreach (Piece p in m_State)
            {
                if ((p & Piece.White) == Piece.White)
                    numWhite += ((p & Piece.King) == Piece.King) ? 3 : 1;
                else if ((p & Piece.Black) == Piece.Black)
                    numBlack += ((p & Piece.King) == Piece.King) ? 3 : 1;
            }
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
        private void ClearLists()
        {
            m_Lefties.Clear();
            m_Righties.Clear();
            m_Kings.Clear();
            m_Kingdirns.Clear();
        }
        Piece[] m_State;
        MoveType m_Move;
        IPiececontroller m_Pc;
        List<Position> m_Righties;           // to move or jump to the right
        List<Position> m_Lefties;            // to move or jump to the left
        List<Position> m_Kings;              // Kings that can move or jump
        List<Direction> m_Kingdirns;         // where the kings can go
    }
    //===============================================================================
    /// <summary>
    /// The game class that maintains the board and a tree of nodes, and facilitates 
    /// player and AI moves.
    /// </summary>
    //===============================================================================
    public class Gameplay
    {
        /// <summary> 
        /// Depth should be an odd number.
        /// 'boardSize' is incremented if odd number.
        /// </summary>
        public Gameplay(int depth, int boardSize)
        {
            C.BoardSize = boardSize;
            m_Depth = depth;
            m_Cg = new Childgetter();
            m_ColorInitialized = false;
            m_Rand = new Random();

            Piece[] board;
            Aux.Initialize(out board);
            m_Root = new Node(board);
        }
        public Gameplay() : this(9, 8)
        { }
        /// <summary> 
        /// Player side. Can be set only once and accessed only when set.
        /// </summary>
        public bool WhitePlayer
        {
            get
            {
                if (m_ColorInitialized)
                    return !m_WhiteAI;
                else
                    throw new InvalidOperationException("Player color not initialized");
            }
            set
            {
                if (!m_ColorInitialized)
                {
                    m_ColorInitialized = true;
                    m_WhiteAI = !value;
                }
                else
                    throw new InvalidOperationException("Player color already initialized");
            }
        }
        /// <summary> Accessing piece at the given position. </summary>
        public Piece this[int row, int col]
        {
            get
            {
                if ((row + col) % 2 == 1)
                    return Piece.Empty;
                else
                    return m_Root.Board[new Position(row, col)];
            }
        }
        /// <summary> Returns 'false' if no possible moves. </summary>
        public bool AITurn()
        {
            if (!m_ColorInitialized)
                throw new InvalidOperationException();

            AlphaBetaPruning(m_Root, m_Depth);

            if (m_Root.ChildNodes.Count == 0)
                return false;
            
            int bestChildInd = 0;
            for (int i = 0; i < m_Root.ChildNodes.Count; ++i)
            {
                if (m_Root.ChildNodes[bestChildInd].Value < m_Root.ChildNodes[i].Value)
                    bestChildInd = i;
            }
            m_Root = m_Root.ChildNodes[bestChildInd];

            return true;
        }
        /// <summary> 
        /// Returns 'false' if impossible to perform the move.
        /// 'row' and 'col' between 0 and boardSize-1.
        /// </summary>
        public bool PlayerTurn(int row, int col, Direction dirn)
        {
            if (!m_ColorInitialized)
                throw new InvalidOperationException();

            Position pos = new Position(row, col);
            bool playerColor = (m_WhiteAI) ? C.Black : C.White;
            m_Cg.Configure(playerColor, m_Root.Board);
            bool jumping = m_Cg.Jumping;

            if ((playerColor == C.White && (m_Root.Board[pos] & Piece.Black) == Piece.Black) ||
                (playerColor == C.Black && ((m_Root.Board[pos] & Piece.White) == Piece.White)))
                return false;

            if ((m_Root.Board[pos] & Piece.King) == Piece.King)
            {
                Direction whereCanGo = (jumping) ? m_Cg.Controller.CanJumpKing(pos) : m_Cg.Controller.CanMoveKing(pos);
                if ((whereCanGo & dirn) == dirn)
                {
                    if (jumping)
                        m_Cg.Controller.JumpKing(dirn, pos);
                    else
                        m_Cg.Controller.MoveKing(dirn, pos);
                }
                else
                    return false;
            }
            else // not a king
            {
                if (jumping)
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
            if (m_Root.ChildNodes != null)
            {
                foreach (Node child in m_Root.ChildNodes)
                    if (Enumerable.SequenceEqual(m_Root.Board, child.Board))
                    {
                        m_Root = child;
                        break;
                    }
            }
            return true;
        }
        private void AlphaBetaPruning(Node node, int depth, double alpha = 0.0, double beta = 1.0, bool aiturn = true)
        {
            if (node.ChildNodes == null || depth == 0)
            {
                bool color;
                if (m_WhiteAI)
                    color = (aiturn) ? C.White : C.Black;
                else
                    color = (aiturn) ? C.Black : C.White;
                m_Cg.Configure(color, node.Board);

                if (depth == 0)
                {
                    node.Value = m_Cg.HeuristicValue(m_WhiteAI);
                    return;
                }
                node.ChildNodes = m_Cg.ChildStates;
                node.ChildNodes.OrderBy(
                    delegate (Node n) { return m_Rand.Next(); }
                );
            }
            if (node.ChildNodes.Count == 0) 
            {
                node.Value = (aiturn) ? 1.0 : 0.0;
                return;
            }

            if (aiturn)
            {
                node.Value = 0.0;
                foreach(Node child in node.ChildNodes)
                {
                    AlphaBetaPruning(child, depth - 1, alpha, beta, false);
                    node.Value = Aux.Max(node.Value, child.Value);
                    alpha = Aux.Max(alpha, node.Value);
                    if (beta <= alpha)
                        return;
                }
            }
            else
            {
                node.Value = 1.0;
                foreach(Node child in node.ChildNodes)
                {
                    AlphaBetaPruning(child, depth - 1, alpha, beta, true);
                    node.Value = Aux.Min(node.Value, child.Value);
                    beta = Aux.Min(beta, node.Value);
                    if (beta <= alpha)
                        return;
                }
            }
        }
        int         m_Depth;
        Childgetter m_Cg;
        bool        m_WhiteAI;
        bool        m_ColorInitialized;
        Node        m_Root;
        Random      m_Rand;
    }
}