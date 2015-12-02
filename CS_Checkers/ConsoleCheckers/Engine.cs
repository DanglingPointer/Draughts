using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Checkers
{
    //===============================================================================
    public static class Constants
    {
        public const ushort BoardSize = 8;
        public static T Max<T>(params T[] args) where T :IComparable<T>
        {
            T max = args[0];
            foreach(T arg in args)
            {
                if (arg.CompareTo(max) > 0)
                    max = arg;
            }
            return max;
        }
    }
    [Flags]
    public enum Piece : byte
    {
        Empty = 0x0,
        White = 0x1,
        Black = 0x2,
        King  = 0x4
    }
    public struct Pos
    {
        public Pos(int row, int col)
        {
            if (row >= Constants.BoardSize || col >= Constants.BoardSize
                || row < 0 || col < 0) 
                throw new IndexOutOfRangeException();
            m_Row = Convert.ToUInt16(row);
            m_Col = Convert.ToUInt16(col);
        }
        public ushort Row
        {
            get { return m_Row; }
        }
        public ushort Col
        {
            get { return m_Col; }
        }
        ushort m_Row;
        ushort m_Col;
    }
    //===============================================================================
    /// <summary>
    /// Wrapper around byte-field Piece[]. Performs elementary operations on pieces.
    /// </summary>
    //===============================================================================
    public class BoardBuilder
    {
        public BoardBuilder()
        {
            m_Data = null;
            m_WhitePos = new List<Pos>();
            m_BlackPos = new List<Pos>();
            m_Log = new Stack<Operation>();
        }
        public Piece[] Field
        {
            get { return m_Data; }
            set
            {
                m_Data = value;
                m_WhitePos.Clear();
                m_BlackPos.Clear();
                m_Log.Clear();
                for (ushort row = 0; row < Constants.BoardSize; ++row)
                    for (ushort col = 0; col < Constants.BoardSize; ++col)
                    {
                        Piece p = m_Data[Constants.BoardSize * row + col];
                        if ((p & Piece.White) == Piece.White)
                            m_WhitePos.Add(new Pos(row, col));
                        else if ((p & Piece.Black) == Piece.Black)
                            m_BlackPos.Add(new Pos(row, col));
                    }
            }
        }
        //------------------------------------------------------
        // Rows enumeration: Bottom -> Top
        // Columns enumeration: Left -> Right
        public Piece GetAt(Pos pos)
        {
            return m_Data[Constants.BoardSize * pos.Row + pos.Col];
        }
        public Piece GetAt(ushort row, ushort col)
        {
            return GetAt(new Pos(row, col));
        }
        public Piece GetAt(int row, int col)
        {
            ushort urow = Convert.ToUInt16(row);
            ushort ucol = Convert.ToUInt16(col);
            return GetAt(new Pos(urow, ucol));
        }
        public Piece[] Copy
        {
            get
            {
                int length = m_Data.Length;
                Piece[] copy = new Piece[length];
                Array.Copy(m_Data, copy, length);
                return copy;
            }
        }
        public Pos[] WhitePositions
        {
            get { return m_WhitePos.ToArray(); }
        }
        public Pos[] BlackPositions
        {
            get { return m_BlackPos.ToArray(); }
        }
        public int Counter
        {
            get { return m_Log.Count; }
        }
        //------------------------------------------------------
        public void ResetCounter()
        {
            m_Log.Clear();
        }
        public void Initialize()
        {
            for (int i = 0; i < m_Data.Length; ++i)
            {
                m_Data[i] = Piece.Empty;
            }
            for (ushort row = 0; row < Constants.BoardSize / 2 - 1; ++row)
            {   // Placing white pieces
                ushort col = 0;
                if (row % 2 == 0) col = 1;
                while (col < Constants.BoardSize)
                {
                    m_Data[Constants.BoardSize * row + col] = Piece.White;
                    m_WhitePos.Add(new Pos(row, col));
                    col += 2;
                }
            }
            for (ushort row = Constants.BoardSize-1; row > Constants.BoardSize / 2; ++row)
            {   // Placing black pieces
                ushort col = 0;
                if (row % 2 == 0) col = 1;
                while (col < Constants.BoardSize)
                {
                    m_Data[Constants.BoardSize * row + col] = Piece.Black;
                    m_BlackPos.Add(new Pos(row, col));
                    col += 2;
                }
            }
        }
        public void SetAt(Pos pos, Piece p)
        {
            Replace(pos, p, true);
        }
        public void SetAt(ushort row, ushort col, Piece p)
        {
            Replace(new Pos(row, col), p, true);
        }
        public void SetAt(int row, int col, Piece p)
        {
            ushort urow = Convert.ToUInt16(row);
            ushort ucol = Convert.ToUInt16(col);
            Replace(new Pos(urow, ucol), p, true);
        }
        public bool Undo(ushort steps = 1)
        {
            for (ushort n = steps; n > 0; --n)
            {
                if (m_Log.Count == 0)
                    return false;
                Operation op = m_Log.Pop();
                Replace(op.Position, op.Previous, false);
            }
            return true;
        }
        public void ResetState()
        {
            int i = m_Log.Count;
            Undo(Convert.ToUInt16(i));
        }
        //------------------------------------------------------
        //------------------------------------------------------
        private struct Operation
        {
            public Operation(Piece p, Pos pos)
            {
                m_Prev = p; m_Pos = pos;
            }
            public Piece Previous
            {
                get { return m_Prev; }
            }
            public Pos Position
            {
                get { return m_Pos; }
            }
            Piece m_Prev;
            Pos m_Pos;
        }
        private void Replace(Pos pos, Piece pnew, bool record)
        {
            Piece pprev = m_Data[Constants.BoardSize * pos.Row + pos.Col];
            m_Data[Constants.BoardSize * pos.Row + pos.Col] = pnew;

            if (record)
                m_Log.Push(new Operation(pprev, pos));

            if ((pprev & Piece.White) == Piece.White)
                m_WhitePos.Remove(pos);
            else if ((pprev & Piece.Black) == Piece.Black)
                m_BlackPos.Remove(pos);

            if ((pnew & Piece.White) == Piece.White)
                m_WhitePos.Add(pos);
            else if ((pnew & Piece.Black) == Piece.Black)
                m_BlackPos.Add(pos);
        }
        //------------------------------------------------------
        Piece[] m_Data;
        List<Pos> m_WhitePos;
        List<Pos> m_BlackPos;
        Stack<Operation> m_Log;
    }
    //===============================================================================
    [Flags]
    public enum Direction : byte
    {
        None    = 0,
        RightUp = 1,
        LeftUp  = 2,
        RightDown = 4,
        LeftDown = 8
    }
    //===============================================================================
    /// <summary>
    /// Moves right pieces to right positions if possible
    /// </summary>
    //===============================================================================
    public interface Piececontroller
    {
        /// <summary>
        /// Doesn't set if null
        /// </summary>
        BoardBuilder Builder { get; set; }
        /// <summary>
        /// Returns value from 0.0 (loss) to 1.0 (win), 0.5 is a draw
        /// </summary>
        double EvaluateBoard();
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanJumpRight(Pos pos);
        /// <summary>
        /// Whatever is at 'pos' always jumps to the right, possible multiple jumps
        /// </summary>
        void JumpRight(Pos pos);
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanJumpLeft(Pos pos);
        /// <summary>
        /// Whatever is at 'pos' always jumps to the left, possible multiple jumps
        /// </summary>
        void JumpLeft(Pos pos);
        /// <summary>
        /// Might return mutiple directions
        /// </summary>
        Direction CanJumpKing(Pos pos);
        /// <summary>
        /// Doesn't jump if invalide direction
        /// </summary>
        void JumpKing(Direction dirn, Pos pos);
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanMoveRight(Pos pos);
        /// <summary>
        /// Whatever is at 'pos' always moves to the right
        /// </summary>
        void MoveRight(Pos pos);
        /// <summary>
        /// Doesn't check what's at 'pos'
        /// </summary>
        bool CanMoveLeft(Pos pos);
        /// <summary>
        /// Whatever is at 'pos' always moves to the left
        /// </summary>
        void MoveLeft(Pos pos);
        /// <summary>
        /// Might return mutiple directions
        /// </summary>
        Direction CanMoveKing(Pos pos);
        /// <summary>
        /// Moves only if 'dirn' is a valid direction
        /// </summary>
        void KingMove(Direction dirn, Pos pos);
    }
    //===============================================================================
    internal class WhitePiececontroller : Piececontroller
    {
        public WhitePiececontroller(BoardBuilder b)
        {
            m_Builder = b;
        }
        public BoardBuilder Builder
        {
            get { return m_Builder; }
            set { if (value != null) m_Builder = value; }
        }
        public double EvaluateBoard()
        {
            int numWhite = m_Builder.WhitePositions.Length;
            int numBlack = m_Builder.BlackPositions.Length;
            return (double)numWhite / (numWhite + numBlack);
        }
        // Doesn't check 'pos'
        public bool CanJumpRight(Pos pos)
        {
            if (pos.Col > Constants.BoardSize - 3 || pos.Row > Constants.BoardSize - 3)
                return false;
            if ((m_Builder.GetAt(pos.Row + 1, pos.Col + 1) & Piece.Black) == Piece.Black &&
                m_Builder.GetAt(pos.Row + 2, pos.Col + 2) == Piece.Empty)
                return true;
            return false;
        }
        // Doesn't check anything
        public void JumpRight(Pos pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            m_Builder.SetAt(pos.Row + 1, pos.Col + 1, Piece.Empty);
            Pos newpos = new Pos(pos.Row + 2, pos.Col + 2);
            if (newpos.Row == Constants.BoardSize - 1)
                m_Builder.SetAt(newpos, (Piece.White | Piece.King));
            else
            {
                m_Builder.SetAt(newpos, mypiece);

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(new Pos(newpos.Row + 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(new Pos(newpos.Row + 2, newpos.Col - 2)) + 1;

                if (numrightjump != 0 || numrightjump != 0)
                {   // Multiple jumps
                    if (numrightjump > numleftjump)
                        JumpRight(newpos);
                    else
                        JumpLeft(newpos);
                }
            }
        }
        public bool CanJumpLeft(Pos pos)
        {
            if (pos.Col < 2 || pos.Row > Constants.BoardSize - 3)
                return false;
            if ((m_Builder.GetAt(pos.Row + 1, pos.Col - 1) & Piece.Black) == Piece.Black &&
                m_Builder.GetAt(pos.Row + 2, pos.Col - 2) == Piece.Empty)
                return true;
            return false;
        }
        public void JumpLeft(Pos pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            m_Builder.SetAt(pos.Row + 1, pos.Col - 1, Piece.Empty);
            Pos newpos = new Pos(pos.Row + 2, pos.Col - 2);
            if (newpos.Row == Constants.BoardSize - 1)
                m_Builder.SetAt(newpos, (Piece.White | Piece.King));
            else
            {
                m_Builder.SetAt(newpos, mypiece);

                int numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(new Pos(newpos.Row + 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(new Pos(newpos.Row + 2, newpos.Col - 2)) + 1;

                if (numrightjump != 0 || numleftjump != 0)
                {   // Multiple jumps
                    if (numrightjump > numleftjump)
                        JumpRight(newpos);
                    else
                        JumpLeft(newpos);
                }
            }
        }
        public Direction CanJumpKing(Pos pos)
        {
            Direction where = Direction.None;
            if (pos.Row < Constants.BoardSize - 2)
            {
                if (pos.Col < Constants.BoardSize - 2 &&
                    (m_Builder.GetAt(pos.Row + 1, pos.Col + 1) & Piece.Black) == Piece.Black &&
                    (m_Builder.GetAt(pos.Row + 2, pos.Col + 2) == Piece.Empty))
                    where |= Direction.RightUp;
                if (pos.Col > 1 &&
                    (m_Builder.GetAt(pos.Row + 1, pos.Col - 1) & Piece.Black) == Piece.Black &&
                    (m_Builder.GetAt(pos.Row + 2, pos.Col - 2) == Piece.Empty))
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < Constants.BoardSize - 2 &&
                    (m_Builder.GetAt(pos.Row - 1, pos.Col + 1) & Piece.Black) == Piece.Black &&
                    (m_Builder.GetAt(pos.Row - 2, pos.Col + 2) == Piece.Empty))
                    where |= Direction.RightDown;
                if (pos.Col > 1 &&
                    (m_Builder.GetAt(pos.Row - 1, pos.Col - 1) & Piece.Black) == Piece.Black &&
                    (m_Builder.GetAt(pos.Row - 2, pos.Col - 2) == Piece.Empty))
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void JumpKing(Direction dirn, Pos pos)
        {
            Direction nothere = Direction.None;
            Pos newpos;
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);

            if (dirn == Direction.RightUp)
            {
                nothere = Direction.LeftDown;
                m_Builder.SetAt(pos.Row + 1, pos.Col + 1, Piece.Empty);
                newpos = new Pos(pos.Row + 2, pos.Col + 2);
            }
            else if (dirn == Direction.LeftUp)
            {
                nothere = Direction.RightDown;
                m_Builder.SetAt(pos.Row + 1, pos.Col - 1, Piece.Empty);
                newpos = new Pos(pos.Row + 2, pos.Col - 2);
            }
            else if (dirn == Direction.RightDown)
            {
                nothere = Direction.LeftUp;
                m_Builder.SetAt(pos.Row - 1, pos.Col + 1, Piece.Empty);
                newpos = new Pos(pos.Row - 2, pos.Col + 2);
            }
            else if (dirn == Direction.LeftDown)
            {
                nothere = Direction.RightUp;
                m_Builder.SetAt(pos.Row - 1, pos.Col - 1, Piece.Empty);
                newpos = new Pos(pos.Row - 2, pos.Col - 2);
            }
            else
                return;
            m_Builder.SetAt(newpos, mypiece);


            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction newdirn = CanJumpKing(pos);
            if (nothere != Direction.RightUp && (newdirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(new Pos(pos.Row + 2, pos.Col + 2), Direction.LeftDown) + 1;
            if (nothere != Direction.LeftUp && (newdirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(new Pos(pos.Row + 2, pos.Col - 2), Direction.RightDown) + 1;
            if (nothere != Direction.RightDown && (newdirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(new Pos(pos.Row - 2, pos.Col + 2), Direction.LeftUp) + 1;
            if (nothere != Direction.LeftDown && (newdirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(new Pos(pos.Row - 2, pos.Col - 2), Direction.RightUp) + 1;

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
        public bool CanMoveRight(Pos pos)
        {
            if (pos.Col > Constants.BoardSize - 2 || pos.Row > Constants.BoardSize - 2)
                return false;
            if (m_Builder.GetAt(pos.Row + 1, pos.Col + 1) == Piece.Empty)
                return true;
            return false;
        }
        public void MoveRight(Pos pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            if (pos.Row + 1 == Constants.BoardSize - 1)
                m_Builder.SetAt(pos.Row + 1, pos.Col + 1, (Piece.White | Piece.King));
            else
                m_Builder.SetAt(pos.Row + 1, pos.Col + 1, mypiece);
        }
        public bool CanMoveLeft(Pos pos)
        {
            if (pos.Col < 1 || pos.Row > Constants.BoardSize - 2)
                return false;
            if (m_Builder.GetAt(pos.Row + 1, pos.Col - 1) == Piece.Empty)
                return true;
            return false;
        }
        public void MoveLeft(Pos pos)
        {
            Piece mypiece = m_Builder.GetAt(pos);
            m_Builder.SetAt(pos, Piece.Empty);
            if (pos.Row + 1 == Constants.BoardSize - 1)
                m_Builder.SetAt(pos.Row + 1, pos.Col - 1, (Piece.White | Piece.King));
            else
                m_Builder.SetAt(pos.Row + 1, pos.Col - 1, mypiece);
        }

        public Direction CanMoveKing(Pos pos)
        {
            Direction where = Direction.None;
            if (pos.Row < Constants.BoardSize - 1)
            {
                if (pos.Col < Constants.BoardSize - 1 && 
                    m_Builder.GetAt(pos.Row + 1, pos.Col + 1) == Piece.Empty)
                    where |= Direction.RightUp;
                if (pos.Col > 0 && 
                    m_Builder.GetAt(pos.Row + 1, pos.Col - 1) == Piece.Empty)
                    where |= Direction.LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < Constants.BoardSize - 1 &&
                    m_Builder.GetAt(pos.Row - 1, pos.Col + 1) == Piece.Empty)
                    where |= Direction.RightDown;
                if (pos.Col > 0 &&
                    m_Builder.GetAt(pos.Row - 1, pos.Col - 1) == Piece.Empty)
                    where |= Direction.LeftDown;
            }
            return where;
        }
        public void KingMove(Direction dirn, Pos pos)
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
        /// <summary>
        /// Returns max number of possible successive jumps from 'pos'
        /// </summary>
        private int MaxJump(Pos pos)
        {
            int rightjump=0, leftjump=0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(new Pos(pos.Row + 2, pos.Col + 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(new Pos(pos.Row + 2, pos.Col - 2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        /// <summary>
        /// Returns max number of possible successive jumps for a King at 'pos'.
        /// 'nothere' is the direction where it cannot jump from 'pos'.
        /// </summary>
        private int MaxJumpKing(Pos pos, Direction nothere)
        {
            int rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction.RightUp && (dirn & Direction.RightUp) == Direction.RightUp)
                rightUp = MaxJumpKing(new Pos(pos.Row + 2, pos.Col + 2), Direction.LeftDown) + 1;
            if (nothere != Direction.LeftUp && (dirn & Direction.LeftUp) == Direction.LeftUp)
                leftUp = MaxJumpKing(new Pos(pos.Row + 2, pos.Col - 2), Direction.RightDown) + 1;
            if (nothere != Direction.RightDown && (dirn & Direction.RightDown) == Direction.RightDown)
                rightDown = MaxJumpKing(new Pos(pos.Row - 2, pos.Col + 2), Direction.LeftUp) + 1;
            if (nothere != Direction.LeftDown && (dirn & Direction.LeftDown) == Direction.LeftDown)
                leftDown = MaxJumpKing(new Pos(pos.Row - 2, pos.Col - 2), Direction.RightUp) + 1;
            return Constants.Max(rightUp, rightDown, leftUp, leftDown);
        }
        BoardBuilder m_Builder;
    }
    //===============================================================================
    internal class BlackPiececontroller : Piececontroller
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
        public double EvaluateBoard()
        {
            int numWhite = m_Builder.WhitePositions.Length;
            int numBlack = m_Builder.BlackPositions.Length;
            return (double)numBlack / (numWhite + numBlack);
        }
        // Doesn't check if pos is White
        public bool CanJumpRight(Pos pos)
        {
            if (pos.Col > Constants.BoardSize - 3 || pos.Row < 2)
                return false;
            if ((m_Builder.GetAt(pos.Row - 1, pos.Col + 1) & Piece.Black) == Piece.Black &&
                m_Builder.GetAt(pos.Row - 2, pos.Col + 2) == Piece.Empty)
                return true;
            return false;
        }
        BoardBuilder m_Builder;
    }
    //===============================================================================
    /// <summary>
    /// Creates an array of child states from the current game state.
    /// To be created only once, then takes different fields.
    /// </summary>
    //===============================================================================
    public class ChildGetter
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
        public Piece[] CurrentState
        {
            get { return m_Build.Field; }
            set
            {
                m_Build.Field = value;

                if (m_Righties == null)
                    m_Righties = new List<Pos>();
                else
                    m_Righties.Clear();

                if (m_Lefties == null)
                    m_Lefties = new List<Pos>();
                else
                    m_Lefties.Clear();

                if (m_Kings == null)
                    m_Kings = new List<Pos>();
                else
                    m_Kings.Clear();

                if (m_Kingdirns == null)
                    m_Kingdirns = new List<Direction>();
                else
                    m_Kingdirns.Clear();

                bool jumpers;
                UpdateMembers(out jumpers);
                m_Move = (jumpers) ? MoveType.Jump : MoveType.Move;
            }
        }
        public List<Piece[]> Childs // TODO
        {
            get { return new List<Piece[]>(); }
        }
        public double HeuristicValue
        {
            get { return m_Pc.EvaluateBoard(); }
        }
        /// <summary>
        /// Either only jumpers ('true') or only movers ('false').
        /// Doesn't clear the lists
        /// </summary>
        private void UpdateMembers(out bool jumpers)
        {
            Pos[] allPositions;
            if (m_Pc is WhitePiececontroller)
                allPositions = m_Build.WhitePositions;
            else if (m_Pc is BlackPiececontroller)
                allPositions = m_Build.BlackPositions;
            else
                throw new InvalidOperationException();

            bool jump = false;
            foreach (Pos p in allPositions)
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
                            jump = true;
                        }
                        m_Kings.Add(p);
                        m_Kingdirns.Add(where);
                    }
                    else
                    {
                        where = m_Pc.CanMoveKing(p);
                        if (where != Direction.None)
                        {
                            m_Kings.Add(p);
                            m_Kingdirns.Add(where);
                        }
                    }
                }
                else
                {
                    if (m_Pc.CanJumpRight(p))
                    {
                        if (!jump)
                        {
                            m_Righties.Clear();
                            jump = true;
                        }
                        m_Righties.Add(p);
                    }
                    if (m_Pc.CanJumpLeft(p))
                    {
                        if (!jump)
                        {
                            m_Lefties.Clear();
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
            jumpers = jump;
        }
        MoveType m_Move;
        Piececontroller m_Pc;
        List<Pos> m_Righties;   // to move or jump to the right
        List<Pos> m_Lefties;    // to move or jump to the left
        List<Pos> m_Kings;      // Kings that can move or jump
        List<Direction> m_Kingdirns;    // where the kings can go
        BoardBuilder m_Build;
    }
}
