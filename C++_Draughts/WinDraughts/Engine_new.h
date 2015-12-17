#pragma once
#include<list>
#include<vector>
#include<cstring>
#include<initializer_list>
#include<stdexcept>
#define BOARD_SIZE 8
#define ARRAY_SIZE (BOARD_SIZE*BOARD_SIZE / 2)
#define WHITE true
#define BLACK false
namespace Draughts
{
    typedef unsigned char byte;
    typedef char sbyte;
    enum Piece :byte
    {
        Empty = 0x0,
        White = 0x1,
        Black = 0x2,
        King = 0x4
    };
    enum Direction : byte
    {
        None = 0x0,
        RightUp = 0x1,
        LeftUp = 0x2,
        RightDown = 0x4,
        LeftDown = 0x8
    };
    template<typename T> inline T operator| (T a, T b)
    {
        return (T)((int)a | (int)b);
    }
    template<typename T> inline T operator& (T a, T b)
    {
        return (T)((int)a & (int)b);
    }
    template<typename T> inline T& operator|= (T& a, T b)
    {
        return (T&)((int&)a |= (int)b);
    }
    template<typename T> inline T& operator&= (T& a, T b)
    {
        return (T&)((int&)a &= (int)b);
    }
    template<typename T> inline T Max(const std::initializer_list<T>& list)
    {
        T max = *(list.begin());
        for (const T *p = list.begin(); p != list.end(); ++p)
            if ((*p) > max)
                max = (*p);
        return max;
    }
    template<typename T> inline T Min(const std::initializer_list<T>& list)
    {
        T min = *(list.begin());
        for (const T *p = list.begin(); p != list.end(); ++p)
            if ((*p) < min)
                min = (*p);
        return min;
    }
    // Creates dynamic array and initializes it
    Piece *Initialize()
    {
        Piece *pboard = new Piece[ARRAY_SIZE]{};
        for (int i = 0; i < ARRAY_SIZE / 2 - ARRAY_SIZE / 2; ++i)
            pboard[i] = Piece::White;
        for (int i = (ARRAY_SIZE / 2 + ARRAY_SIZE / 2); i < ARRAY_SIZE; ++i)
            pboard[i] = Piece::Black;
        return pboard;
    }
    // Copies 'poriginal' into a new dynamic array
    Piece *CreateCopyOf(const Piece *poriginal)
    {
        Piece *pboard = new Piece[ARRAY_SIZE];
        std::memcpy(pboard, poriginal, ARRAY_SIZE * sizeof(byte));
        return pboard;
    }
    // Immutable, convertible
    class Position
    {
    public:
        Position(byte row, byte col) :Row(row), Col(col)
        { }
        Position(byte index) :Row(index / (BOARD_SIZE / 2)), Col((index % (BOARD_SIZE / 2)) * 2 + Row % 2)
        { }
        Position Offset(sbyte rowoffset, sbyte coloffset) const
        {
            return Position(Row + rowoffset, Col + coloffset);
        }
        operator byte() const
        {
            return Row * (BOARD_SIZE / 2) + Col / 2;
        }
        const byte Row;
        const byte Col;
    };
    __interface IPiececontroller
    {
        Piece *GetBoard() const;
        void SetBoard(Piece *pboard);

        std::list<Position> GetPositions() const;

        bool CanJumpRight(const Position& pos) const;
        void JumpRight(const Position& pos) const;
        bool CanJumpLeft(const Position& pos) const;
        void JumpLeft(const Position& pos) const;

        Direction CanJumpKing(const Position& pos) const;
        void JumpKing(Direction dirn, const Position& pos) const;

        bool CanMoveRight(const Position& pos) const;
        void MoveRight(const Position& pos) const;
        bool CanMoveLeft(const Position& pos) const;
        void MoveLeft(const Position& pos) const;

        Direction CanMoveKing(const Position& pos) const;
        void MoveKing(Direction dirn, const Position& pos) const;
    };
    class WhitePiececontroller : public IPiececontroller
    {
    public:
        WhitePiececontroller(Piece *pboard) :m_pBoard(pboard)
        { }
        Piece *GetBoard() const
        {
            return m_pBoard;
        }
        void SetBoard(Piece *pboard)
        {
            m_pBoard = pboard;
        }
        std::list<Position> GetPositions() const
        {
            std::list<Position> posns;
            for (byte i = 0; i < ARRAY_SIZE; i++)
                if (m_pBoard[i] & Piece::White)
                    posns.push_back(Position(i));
            return posns;
        }
        bool CanJumpRight(const Position& pos) const    // converted
        {
            if (pos.Col > BOARD_SIZE - 3 || pos.Row > BOARD_SIZE - 3)
                return false;
            if ((m_pBoard[pos.Offset(1, 1)] & Piece::Black) &&
                m_pBoard[pos.Offset(2, 2)] == Piece::Empty)
                return true;
            return false;
        }
        void JumpRight(const Position& pos) const   // converted
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            m_pBoard[pos.Offset(1, 1)] = Piece::Empty;
            Position newpos = pos.Offset(2, 2);
            if (newpos.Row == BOARD_SIZE - 1)
                m_pBoard[newpos] = (Piece::White | Piece::King);
            else
            {
                m_pBoard[newpos] = mypiece;

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
        bool CanJumpLeft(const Position& pos) const // converted
        {
            if (pos.Col < 2 || pos.Row > BOARD_SIZE - 3)
                return false;
            if ((m_pBoard[pos.Offset(1, -1)] & Piece::Black) == Piece::Black &&
                m_pBoard[pos.Offset(2, -2)] == Piece::Empty)
                return true;
            return false;
        }
        void JumpLeft(const Position& pos) const    // converted
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            m_pBoard[pos.Offset(1, -1)] = Piece::Empty;
            Position newpos = pos.Offset(2, -2);
            if (newpos.Row == BOARD_SIZE - 1)
                m_pBoard[newpos] = (Piece::White | Piece::King);
            else
            {
                m_pBoard[newpos] = mypiece;

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
        Direction CanJumpKing(const Position& pos) const    // converted
        {
            Direction where = Direction::None;
            if (pos.Row < BOARD_SIZE - 2)
            {
                if (pos.Col < BOARD_SIZE - 2 &&
                    (m_pBoard[pos.Offset(1, 1)] & Piece::Black) == Piece::Black &&
                    m_pBoard[pos.Offset(2, 2)] == Piece::Empty)
                where |= Direction::RightUp;
                if (pos.Col > 1 &&
                    (m_pBoard[pos.Offset(1, -1)] & Piece::Black) == Piece::Black &&
                    (m_pBoard[pos.Offset(2, -2)] == Piece::Empty))
                where |= Direction::LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < BOARD_SIZE - 2 &&
                    (m_pBoard[pos.Offset(-1, 1)] & Piece::Black) == Piece::Black &&
                    (m_pBoard[pos.Offset(-2, 2)] == Piece::Empty))
                where |= Direction::RightDown;
                if (pos.Col > 1 &&
                    (m_pBoard[pos.Offset(-1, -1)] & Piece::Black) == Piece::Black &&
                    (m_pBoard[pos.Offset(-2, -2)] == Piece::Empty))
                where |= Direction::LeftDown;
            }
            return where;
        }
        void JumpKing(Direction dirn, const Position& pos) const    // converted
        {
            Position *pnewpos;
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;

            if (dirn == Direction::RightUp)
            {
                m_pBoard[pos.Offset(1, 1)] = Piece::Empty;
                pnewpos = new Position(pos.Row + 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftUp)
            {
                m_pBoard[pos.Offset(1, -1)] = Piece::Empty;
                pnewpos = new Position(pos.Row + 2, pos.Col - 2);
            }
            else if (dirn == Direction::RightDown)
            {
                m_pBoard[pos.Offset(-1, 1)] = Piece::Empty;
                pnewpos = new Position(pos.Row - 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftDown)
            {
                m_pBoard[pos.Offset(-1, -1)] = Piece::Empty;
                pnewpos = new Position(pos.Row - 2, pos.Col - 2);
            }
            else
                return;
            m_pBoard[*pnewpos] = mypiece;


            byte rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction newdirn = CanJumpKing(*pnewpos);
            if ((newdirn & Direction::RightUp) == Direction::RightUp)
                rightUp = MaxJumpKing(Position(pnewpos->Row + 2, pnewpos->Col + 2), Direction::LeftDown) + 1;
            if ((newdirn & Direction::LeftUp) == Direction::LeftUp)
                leftUp = MaxJumpKing(Position(pnewpos->Row + 2, pnewpos->Col - 2), Direction::RightDown) + 1;
            if ((newdirn & Direction::RightDown) == Direction::RightDown)
                rightDown = MaxJumpKing(Position(pnewpos->Row - 2, pnewpos->Col + 2), Direction::LeftUp) + 1;
            if ((newdirn & Direction::LeftDown) == Direction::LeftDown)
                leftDown = MaxJumpKing(Position(pnewpos->Row - 2, pnewpos->Col - 2), Direction::RightUp) + 1;

            if (rightDown != 0 || rightUp != 0 || leftDown != 0 || leftUp != 0)
            {
                if (rightUp >= rightDown && rightUp >= leftUp && rightUp >= leftDown)
                    JumpKing(Direction::RightUp, *pnewpos);
                else if (rightDown >= rightUp && rightDown >= leftUp && rightDown >= leftDown)
                    JumpKing(Direction::RightDown, *pnewpos);
                else if (leftDown >= rightDown && leftDown >= leftUp && leftDown >= rightUp)
                    JumpKing(Direction::LeftDown, *pnewpos);
                else
                    JumpKing(Direction::LeftUp, *pnewpos);
            }
            delete pnewpos;
        }
        bool CanMoveRight(const Position& pos) const    // converted
        {
            if (pos.Col > BOARD_SIZE - 2 || pos.Row > BOARD_SIZE - 2)
                return false;
            if (m_pBoard[pos.Offset(1, 1)] == Piece::Empty)
                return true;
            return false;
        }
        void MoveRight(const Position& pos) const   // converted
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            if (pos.Row + 1 == BOARD_SIZE - 1)
                m_pBoard[pos.Offset(1, 1)] = (Piece::White | Piece::King);
            else
                m_pBoard[pos.Offset(1, 1)] = mypiece;
        }
        bool CanMoveLeft(const Position& pos) const // converted
        {
            if (pos.Col < 1 || pos.Row > BOARD_SIZE - 2)
                return false;
            if (m_pBoard[pos.Offset(1, -1)] == Piece::Empty)
                return true;
            return false;
        }
        void MoveLeft(const Position& pos) const    // converted
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            if (pos.Row + 1 == BOARD_SIZE - 1)
                m_pBoard[pos.Offset(1, -1)] = (Piece::White | Piece::King);
            else
                m_pBoard[pos.Offset(1, -1)] = mypiece;
        }
        Direction CanMoveKing(const Position& pos) const    // converted
        {
            Direction where = Direction::None;
            if (pos.Row < BOARD_SIZE - 1)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBoard[pos.Offset(1, 1)] == Piece::Empty)
                where |= Direction::RightUp;
                if (pos.Col > 0 &&
                    m_pBoard[pos.Offset(1, -1)] == Piece::Empty)
                where |= Direction::LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBoard[pos.Offset(-1, 1)] == Piece::Empty)
                where |= Direction::RightDown;
                if (pos.Col > 0 &&
                    m_pBoard[pos.Offset(-1, -1)] == Piece::Empty)
                where |= Direction::LeftDown;
            }
            return where;
        }
        void MoveKing(Direction dirn, const Position& pos) const    // converted
        {
            if (dirn == Direction::None)
                return;
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            if ((dirn & Direction::RightUp) == Direction::RightUp)
                m_pBoard[pos.Offset(1, 1)] = mypiece;
            else if ((dirn & Direction::LeftUp) == Direction::LeftUp)
                m_pBoard[pos.Offset(1, -1)] = mypiece;
            else if ((dirn & Direction::RightDown) == Direction::RightDown)
                m_pBoard[pos.Offset(-1, 1)] = mypiece;
            else if ((dirn & Direction::LeftDown) == Direction::LeftDown)
                m_pBoard[pos.Offset(-1, -1)] = mypiece;
        }
    private:
        byte MaxJump(const Position& pos) const // converted
        {
            byte rightjump = 0, leftjump = 0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(pos.Offset(2, 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(pos.Offset(2, -2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        byte MaxJumpKing(const Position& pos, Direction nothere) const  // converted
        {
            byte rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction::RightUp && (dirn & Direction::RightUp) == Direction::RightUp)
                rightUp = MaxJumpKing(pos.Offset(2, 2), Direction::LeftDown) + 1;
            if (nothere != Direction::LeftUp && (dirn & Direction::LeftUp) == Direction::LeftUp)
                leftUp = MaxJumpKing(pos.Offset(2, -2), Direction::RightDown) + 1;
            if (nothere != Direction::RightDown && (dirn & Direction::RightDown) == Direction::RightDown)
                rightDown = MaxJumpKing(pos.Offset(-2, 2), Direction::LeftUp) + 1;
            if (nothere != Direction::LeftDown && (dirn & Direction::LeftDown) == Direction::LeftDown)
                leftDown = MaxJumpKing(pos.Offset(-2, -2), Direction::RightUp) + 1;
            return Max({ rightUp, rightDown, leftUp, leftDown });
        }
        Piece *m_pBoard;
    };
    class BlackPiececontroller : public IPiececontroller
    {
    public:
        BlackPiececontroller(Piece *pboard) :m_pBoard(pboard)
        { }
        Piece *GetBoard() const
        {
            return m_pBoard;
        }
        void SetBoard(Piece *pboard)
        {
            m_pBoard = pboard;
        }
        std::list<Position> GetPositions() const
        {
            std::list<Position> posns;
            for (byte i = 0; i < ARRAY_SIZE; i++)
                if (m_pBoard[i] & Piece::Black)
                    posns.push_back(Position(i));
            return posns;
        }
        bool CanJumpRight(const Position& pos) const
        {
            if (pos.Col > BOARD_SIZE - 3 || pos.Row < 2)
                return false;
            if ((m_pBoard[pos.Offset(-1, 1)] & Piece::White) == Piece::White &&
                m_pBoard[pos.Offset(-2, 2)] == Piece::Empty)
                return true;
            return false;
        }
        void JumpRight(const Position& pos) const
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            m_pBoard[pos.Offset(-1, 1)] = Piece::Empty;
            Position newpos = pos.Offset(-2, 2);
            if (newpos.Row == 0)
                m_pBoard[newpos] = (Piece::Black | Piece::King);
            else
            {
                m_pBoard[newpos] = mypiece;

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
        bool CanJumpLeft(const Position& pos) const
        {
            if (pos.Col < 2 || pos.Row < 2)
                return false;
            if ((m_pBoard[pos.Offset(-1, -1)] & Piece::White) == Piece::White &&
                m_pBoard[pos.Offset(-2, -2)] == Piece::Empty)
                return true;
            return false;
        }
        void JumpLeft(const Position& pos) const
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            m_pBoard[pos.Offset(-1, -1)] = Piece::Empty;
            Position newpos = pos.Offset(-2, -2);
            if (newpos.Row == 0)
                m_pBoard[newpos] = (Piece::Black | Piece::King);
            else
            {
                m_pBoard[newpos] = mypiece;

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
        Direction CanJumpKing(const Position& pos) const
        {
            Direction where = Direction::None;
            if (pos.Row < BOARD_SIZE - 2)
            {
                if (pos.Col < BOARD_SIZE - 2 &&
                    (m_pBoard[pos.Offset(1, 1)] & Piece::White) == Piece::White &&
                    m_pBoard[pos.Offset(2, 2)] == Piece::Empty)
                where |= Direction::RightUp;
                if (pos.Col > 1 &&
                    (m_pBoard[pos.Offset(1, -1)] & Piece::White) == Piece::White &&
                    (m_pBoard[pos.Offset(2, -2)] == Piece::Empty))
                where |= Direction::LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < BOARD_SIZE - 2 &&
                    (m_pBoard[pos.Offset(-1, 1)] & Piece::White) == Piece::White &&
                    (m_pBoard[pos.Offset(-2, 2)] == Piece::Empty))
                where |= Direction::RightDown;
                if (pos.Col > 1 &&
                    (m_pBoard[pos.Offset(-1, -1)] & Piece::White) == Piece::White &&
                    (m_pBoard[pos.Offset(-2, -2)] == Piece::Empty))
                where |= Direction::LeftDown;
            }
            return where;
        }
        void JumpKing(Direction dirn, const Position& pos) const
        {
            Position *pnewpos;
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;

            if (dirn == Direction::RightUp)
            {
                m_pBoard[pos.Offset(1, 1)] = Piece::Empty;
                pnewpos = new Position(pos.Row + 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftUp)
            {
                m_pBoard[pos.Offset(1, -1)] = Piece::Empty;
                pnewpos = new Position(pos.Row + 2, pos.Col - 2);
            }
            else if (dirn == Direction::RightDown)
            {
                m_pBoard[pos.Offset(-1, 1)] = Piece::Empty;
                pnewpos = new Position(pos.Row - 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftDown)
            {
                m_pBoard[pos.Offset(-1, -1)] = Piece::Empty;
                pnewpos = new Position(pos.Row - 2, pos.Col - 2);
            }
            else
                return;
            m_pBoard[*pnewpos] = mypiece;

            byte rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction newdirn = CanJumpKing(*pnewpos);
            if ((newdirn & Direction::RightUp) == Direction::RightUp)
                rightUp = MaxJumpKing(Position(pnewpos->Row + 2, pnewpos->Col + 2), Direction::LeftDown) + 1;
            if ((newdirn & Direction::LeftUp) == Direction::LeftUp)
                leftUp = MaxJumpKing(Position(pnewpos->Row + 2, pnewpos->Col - 2), Direction::RightDown) + 1;
            if ((newdirn & Direction::RightDown) == Direction::RightDown)
                rightDown = MaxJumpKing(Position(pnewpos->Row - 2, pnewpos->Col + 2), Direction::LeftUp) + 1;
            if ((newdirn & Direction::LeftDown) == Direction::LeftDown)
                leftDown = MaxJumpKing(Position(pnewpos->Row - 2, pnewpos->Col - 2), Direction::RightUp) + 1;

            if (rightDown != 0 || rightUp != 0 || leftDown != 0 || leftUp != 0)
            {
                if (rightUp >= rightDown && rightUp >= leftUp && rightUp >= leftDown)
                    JumpKing(Direction::RightUp, *pnewpos);
                else if (rightDown >= rightUp && rightDown >= leftUp && rightDown >= leftDown)
                    JumpKing(Direction::RightDown, *pnewpos);
                else if (leftDown >= rightDown && leftDown >= leftUp && leftDown >= rightUp)
                    JumpKing(Direction::LeftDown, *pnewpos);
                else
                    JumpKing(Direction::LeftUp, *pnewpos);
            }
            delete pnewpos;
        }
        bool CanMoveRight(const Position& pos) const
        {
            if (pos.Col > BOARD_SIZE - 2 || pos.Row < 1)
                return false;
            if (m_pBoard[pos.Offset(-1, 1)] == Piece::Empty)
                return true;
            return false;
        }
        void MoveRight(const Position& pos) const
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            if (pos.Row - 1 == 0)
                m_pBoard[pos.Offset(-1, 1)] = (Piece::Black | Piece::King);
            else
                m_pBoard[pos.Offset(-1, 1)] = mypiece;
        }
        bool CanMoveLeft(const Position& pos) const
        {
            if (pos.Col < 1 || pos.Row < 1)
                return false;
            if (m_pBoard[pos.Offset(-1, -1)] == Piece::Empty)
                return true;
            return false;
        }
        void MoveLeft(const Position& pos) const
        {
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            if (pos.Row - 1 == 0)
                m_pBoard[pos.Offset(-1, -1)] = (Piece::Black | Piece::King);
            else
                m_pBoard[pos.Offset(-1, -1)] = mypiece;
        }

        Direction CanMoveKing(const Position& pos) const
        {
            Direction where = Direction::None;
            if (pos.Row < BOARD_SIZE - 1)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBoard[pos.Offset(1, 1)] == Piece::Empty)
                where |= Direction::RightUp;
                if (pos.Col > 0 &&
                    m_pBoard[pos.Offset(1, -1)] == Piece::Empty)
                where |= Direction::LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBoard[pos.Offset(-1, 1)] == Piece::Empty)
                where |= Direction::RightDown;
                if (pos.Col > 0 &&
                    m_pBoard[pos.Offset(-1, -1)] == Piece::Empty)
                where |= Direction::LeftDown;
            }
            return where;
        }
        void MoveKing(Direction dirn, const Position& pos) const
        {
            if (dirn == Direction::None)
                return;
            Piece mypiece = m_pBoard[pos];
            m_pBoard[pos] = Piece::Empty;
            if ((dirn & Direction::RightUp) == Direction::RightUp)
                m_pBoard[pos.Offset(1, 1)] = mypiece;
            else if ((dirn & Direction::LeftUp) == Direction::LeftUp)
                m_pBoard[pos.Offset(1, -1)] = mypiece;
            else if ((dirn & Direction::RightDown) == Direction::RightDown)
                m_pBoard[pos.Offset(-1, 1)] = mypiece;
            else if ((dirn & Direction::LeftDown) == Direction::LeftDown)
                m_pBoard[pos.Offset(-1, -1)] = mypiece;
        }
    private:
        byte MaxJump(const Position& pos) const
        {
            byte rightjump = 0, leftjump = 0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(pos.Offset(-2, 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(pos.Offset(-2, -2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        byte MaxJumpKing(const Position& pos, Direction nothere) const  // same as white
        {
            byte rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction::RightUp && (dirn & Direction::RightUp) == Direction::RightUp)
                rightUp = MaxJumpKing(pos.Offset(2, 2), Direction::LeftDown) + 1;
            if (nothere != Direction::LeftUp && (dirn & Direction::LeftUp) == Direction::LeftUp)
                leftUp = MaxJumpKing(pos.Offset(2, -2), Direction::RightDown) + 1;
            if (nothere != Direction::RightDown && (dirn & Direction::RightDown) == Direction::RightDown)
                rightDown = MaxJumpKing(pos.Offset(-2, 2), Direction::LeftUp) + 1;
            if (nothere != Direction::LeftDown && (dirn & Direction::LeftDown) == Direction::LeftDown)
                leftDown = MaxJumpKing(pos.Offset(-2, -2), Direction::RightUp) + 1;
            return Max({ rightUp, rightDown, leftUp, leftDown });
        }
        Piece *m_pBoard;
    };
    class Childgetter
    {
        enum MoveType : byte
        {
            Unset, Move, Jump
        };
    public:
        Childgetter(): m_Move(MoveType::Unset), m_pState(nullptr), m_pPc(nullptr)
        { }
        ~Childgetter()
        {
            if (m_pPc != nullptr)
                delete m_pPc;
        }
        void Configure(bool whitecolor, Piece *pnewstate)
        {
            if (whitecolor)
                m_pPc = new WhitePiececontroller(pnewstate);
            else
                m_pPc = new BlackPiececontroller(pnewstate);

            m_pState = pnewstate;

            m_Righties.clear();
            m_Lefties.clear();
            m_Kings.clear();
            m_Kingdirns.clear();

            std::list<Position> allPositions = m_pPc->GetPositions();

            bool jump = false;
            for (auto it = allPositions.begin(); it != allPositions.end(); ++it)
            {
                Position p = *it;
                if (m_pState[p] & Piece::King)
                {
                    Direction where = m_pPc->CanJumpKing(p);
                    if (where != Direction::None)
                    {
                        if (!jump)
                        {
                            m_Kings.clear();
                            m_Kingdirns.clear();
                            m_Righties.clear();
                            m_Lefties.clear();
                            jump = true;
                        }
                        m_Kings.push_back(p);
                        m_Kingdirns.push_back(where);
                    }
                    else if (!jump && (where = m_pPc->CanMoveKing(p)) != Direction::None)
                    {
                        m_Kings.push_back(p);
                        m_Kingdirns.push_back(where);
                    }
                }
                else // not a king
                {
                    if (m_pPc->CanJumpRight(p))
                    {
                        if (!jump)
                        {
                            m_Kings.clear();
                            m_Kingdirns.clear();
                            m_Righties.clear();
                            m_Lefties.clear();
                            jump = true;
                        }
                        m_Righties.push_back(p);
                    }
                    if (m_pPc->CanJumpLeft(p))
                    {
                        if (!jump)
                        {
                            m_Kings.clear();
                            m_Kingdirns.clear();
                            m_Righties.clear();
                            m_Lefties.clear();
                            jump = true;
                        }
                        m_Lefties.push_back(p);
                    }
                    else if (!jump)
                    {
                        if (m_pPc->CanMoveRight(p)) m_Righties.push_back(p);
                        if (m_pPc->CanMoveLeft(p)) m_Lefties.push_back(p);
                    }
                }
            } // for
            m_Move = (jump) ? MoveType::Jump : MoveType::Move;
        }
        std::vector<Piece*> ChildStates() const
        {
            if (m_Move == MoveType::Unset)
                throw std::logic_error("Childgetter not configured");

            std::vector<Piece*> childs;

            for (auto it = m_Righties.begin(); it != m_Righties.end(); ++it)
            {
                Piece *pchild = CreateCopyOf(m_pState);
                m_pPc->SetBoard(pchild);
                if (m_Move == MoveType::Jump)
                    m_pPc->JumpRight(*it);
                else
                    m_pPc->MoveRight(*it);
                childs.push_back(pchild);
            }
            for (auto it = m_Lefties.begin(); it != m_Lefties.end(); ++it)
            {
                Piece *pchild = CreateCopyOf(m_pState);
                m_pPc->SetBoard(pchild);
                if (m_Move == MoveType::Jump)
                    m_pPc->JumpLeft(*it);
                else
                    m_pPc->MoveLeft(*it);
                childs.push_back(pchild);
            }
            for (unsigned int i = 0; i < m_Kings.size(); ++i)
            {
                Direction dirn = m_Kingdirns[i];
                Position p = m_Kings[i];

                if (m_Move == MoveType::Jump)
                {
                    if (dirn & Direction::LeftDown)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->JumpKing(Direction::LeftDown, p);
                        childs.push_back(pchild);
                    }
                    if (dirn & Direction::LeftUp)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->JumpKing(Direction::LeftUp, p);
                        childs.push_back(pchild);
                    }
                    if (dirn & Direction::RightDown)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->JumpKing(Direction::RightDown, p);
                        childs.push_back(pchild);
                    }
                    if (dirn & Direction::RightUp)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->JumpKing(Direction::RightUp, p);
                        childs.push_back(pchild);
                    }
                }
                else // if (m_Move == MoveType.Move)
                {
                    if (dirn & Direction::LeftDown)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->MoveKing(Direction::LeftDown, p);
                        childs.push_back(pchild);
                    }
                    if (dirn & Direction::LeftUp)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->MoveKing(Direction::LeftUp, p);
                        childs.push_back(pchild);
                    }
                    if (dirn & Direction::RightDown)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->MoveKing(Direction::RightDown, p);
                        childs.push_back(pchild);
                    }
                    if (dirn & Direction::RightUp)
                    {
                        Piece *pchild = CreateCopyOf(m_pState);
                        m_pPc->SetBoard(pchild);
                        m_pPc->MoveKing(Direction::RightUp, p);
                        childs.push_back(pchild);
                    }
                }
            }
            return childs;
        }
        double HeuristicValue(bool white_is_max_side)
        {
            int numWhite = 0;
            int numBlack = 0;
            for (Piece *p = m_pState; p != (m_pState + ARRAY_SIZE); ++p)
            {
                if ((*p) & Piece::White)
                    numWhite += ((*p) & Piece::King) ? 2 : 1;
                else if ((*p) & Piece::Black)
                    numBlack += ((*p) & Piece::King) ? 2 : 1;
            }
            int numerator = (white_is_max_side) ? numWhite : numBlack;
            return (double)numerator / (numWhite + numBlack);
        }
        bool Jumping() const
        {
            return m_Move == MoveType::Jump;
        }
        bool Moving() const
        {
            return m_Move == MoveType::Move;
        }
        const IPiececontroller *Controller() const
        {
            return m_pPc;
        }
    private:
        MoveType m_Move;
        Piece *m_pState;
        IPiececontroller *m_pPc;
        std::list<Position> m_Righties;
        std::list<Position> m_Lefties;
        std::vector<Position> m_Kings;
        std::vector<Direction> m_Kingdirns;
    };
    class Gameplay
    {
    public:
        Gameplay(bool player_is_white, int depth) 
            :m_pBoard(Initialize()), m_Depth(depth), m_WhiteAI(!player_is_white), m_Cg()
        { }
        ~Gameplay()
        {
            delete[] m_pBoard;
        }
        Piece GetPieceAt(byte row, byte col) const
        {
            if ((row + col) % 2 == 1)
                return Piece::Empty;
            else
                return m_pBoard[Position(row, col)];
        }
        bool AITurn()
        {
            bool color = (m_WhiteAI) ? WHITE : BLACK;

            m_Cg.Configure(color, m_pBoard);

            std::vector<Piece*> children = m_Cg.ChildStates();
            if (children.size() == 0)
                return false;

            double *childValues = new double[children.size()];
            byte bestChildInd = 0;
            for (byte i = 0; i < children.size(); ++i)
            {
                childValues[i] = AlphaBetaPruning(children[i], m_Depth);
                if (childValues[bestChildInd] < childValues[i])
                    bestChildInd = i;
            }
            delete m_pBoard;
            m_pBoard = children[bestChildInd];

            for (auto it = children.begin(); it != children.end(); ++it)
                if ((*it) != m_pBoard)
                    delete[](*it);
            delete[] childValues;

            return true;
        }
        bool PlayerTurn(int row, int col, Direction dirn)
        {
            Position pos(row, col);
            bool color = (m_WhiteAI) ? BLACK : WHITE;
            m_Cg.Configure(color, m_pBoard);

            if (m_pBoard[pos] & Piece::King)
            {
                Direction whereCanGo = (m_Cg.Moving()) ? (m_Cg.Controller())->CanMoveKing(pos) : (m_Cg.Controller())->CanJumpKing(pos);
                if ((whereCanGo & dirn) == dirn)
                {
                    if (m_Cg.Moving()) 
                        (m_Cg.Controller())->MoveKing(dirn, pos);
                    else 
                        (m_Cg.Controller())->JumpKing(dirn, pos);
                }
                else
                    return false;
            }
            else // not a king
            {
                if (m_Cg.Jumping())
                {
                    if ((dirn & (Direction::RightUp | Direction::RightDown)) && (m_Cg.Controller())->CanJumpRight(pos))
                        (m_Cg.Controller())->JumpRight(pos);
                    else if ((dirn & (Direction::LeftUp | Direction::LeftDown)) == dirn && (m_Cg.Controller())->CanJumpLeft(pos))
                        (m_Cg.Controller())->JumpLeft(pos);
                    else
                        return false;
                }
                else // moving
                {
                    if ((dirn & (Direction::RightUp | Direction::RightDown)) == dirn && (m_Cg.Controller())->CanMoveRight(pos))
                        (m_Cg.Controller())->MoveRight(pos);
                    else if ((dirn & (Direction::LeftUp | Direction::LeftDown)) == dirn && (m_Cg.Controller())->CanMoveLeft(pos))
                        (m_Cg.Controller())->MoveLeft(pos);
                    else
                        return false;
                }
            }
            return true;
        }
    private:
        double AlphaBetaPruning(Piece *pnode, int depth, double alpha = 0.0, double beta = 1.0, bool aiturn = false)
        {
            bool color;
            if (m_WhiteAI)
                color = (aiturn) ? WHITE : BLACK;
            else
                color = (aiturn) ? BLACK : WHITE;
            m_Cg.Configure(color, pnode);

            double val = m_Cg.HeuristicValue(m_WhiteAI);
            if (depth == 0 || val == 0.0 || val == 1.0)
                return val;

            std::vector<Piece*> children = m_Cg.ChildStates();
            if (aiturn)
            {
                val = 0.0;
                for (auto it = children.begin(); it != children.end(); ++it)
                {
                    val = Max({ val, AlphaBetaPruning((*it), depth - 1, alpha, beta, false) });
                    alpha = Max({ alpha, val });
                    if (beta <= alpha)
                        break;
                }
            }
            else
            {
                val = 1.0;
                for (auto it = children.begin(); it != children.end(); ++it)
                {
                    val = Min({ val, AlphaBetaPruning((*it), depth - 1, alpha, beta, true) });
                    beta = Min({ beta, val });
                    if (beta <= alpha)
                        break;
                }
            }
            for (auto it = children.begin(); it != children.end(); ++it)
                delete[](*it);

            return val;
        }
        Piece      *m_pBoard;
        int         m_Depth;
        bool        m_WhiteAI;
        Childgetter m_Cg;
    };
}