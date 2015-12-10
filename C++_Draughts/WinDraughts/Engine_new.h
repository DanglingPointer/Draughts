#pragma once
#include<list>
#include<stack>
#include<cstring>
#include<initializer_list>
#define BOARD_SIZE 8
#define ARRAY_SIZE BOARD_SIZE*BOARD_SIZE / 2
#define WHITE true
#define BLACK false
#define KING true
#define MAN false
namespace Draughts
{
    typedef unsigned char byte;
    typedef unsigned short ushort;
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
    template<class T> inline T operator~ (T a)
    {
        return (T)~(int)a;
    }
    template<class T> inline T operator| (T a, T b)
    {
        return (T)((int)a | (int)b);
    }
    template<class T> inline T operator& (T a, T b)
    {
        return (T)((int)a & (int)b);
    }
    template<class T> inline T operator^ (T a, T b)
    {
        return (T)((int)a ^ (int)b);
    }
    template<class T> inline T& operator|= (T& a, T b)
    {
        return (T&)((int&)a |= (int)b);
    }
    template<class T> inline T& operator&= (T& a, T b)
    {
        return (T&)((int&)a &= (int)b);
    }
    template<class T> inline T& operator^= (T& a, T b)
    {
        return (T&)((int&)a ^= (int)b);
    }
    template<typename T> inline T Max(const std::initializer_list<T>& list)
    {
        T max = *(list.begin());
        for (T *p = list.begin(); p != list.end(); ++p)
            if ((*p) > max)
                max = (*p);
        return max;
    }
    struct Position
    {
        Position(byte row, byte col):Row(row), Col(col)
        { }
        const byte Row;
        const byte Col;
    };
    class BoardBuilder
    {
    public:
        BoardBuilder() :m_pData(nullptr)
        { }
        Piece *get_Field() const
        {
            return m_pData;
        }
        // Deletes previous field
        void set_Field(Piece *p)
        {
            m_pData = p;
            m_WhitePos.clear();
            m_BlackPos.clear();
            m_Log.clear();
            for (byte i = 0; i < BOARD_SIZE; ++i)
            {
                if (*(m_pData + i) & Piece::White)
                    m_WhitePos.push_back(i);
                else if (*(m_pData + i) & Piece::Black)
                    m_BlackPos.push_back(i);
            }
        }
        Piece GetAt(byte row, byte col) const
        {
            byte pos = row * (BOARD_SIZE / 2) + col / 2;
            return m_pData[pos];
        }
        Piece GetAt(const Position& pos) const
        {
            return GetAt(pos.Row, pos.Col);
        }
        void SetAt(byte row, byte col, Piece p)
        {
            byte pos = row * (BOARD_SIZE / 2) + col / 2;
            Replace(pos, p, true);
        }
        void SetAt(const Position& pos, Piece p)
        {
            SetAt(pos.Row, pos.Col, p);
        }
        // Copies current board to p
        bool Copy(Piece *p) const
        {
            if (m_pData == nullptr)
                return false;

            std::memcpy(p, m_pData, ARRAY_SIZE*sizeof(byte));
            return true;
        }
        const std::list<byte> WhitePositions() const
        {
            return m_WhitePos;
        }
        const std::list<byte> BlackPositions() const
        {
            return m_BlackPos;
        }
        unsigned int Counter() const
        {
            return m_Log.size();
        }
        void ResetCounter()
        {
            m_Log.clear();
        }
        void Reset()
        {
            while (!m_Log.empty())
            {
                Operation op = m_Log.back();
                m_Log.pop_back();
                Replace(op.Position, op.Previous, false);
            }
        }
        // Initializes p[BOARD_SIZE]
        static void Initialize(Piece *p)
        {
            for (byte i = 0; i < 32; ++i)
                p[i] = Piece::Empty;
            for (byte i = 0; i < 12; ++i)
                p[i] = Piece::White;
            for (byte i = 20; i < 32; ++i)
                p[i] = Piece::Black;
        }
    private:
        void Replace(byte pos, Piece pnew, bool record)
        {
            Piece prev = m_pData[pos];
            m_pData[pos] = pnew;

            if (record)
                m_Log.push_back(Operation(prev, pos));

            if (prev & Piece::White)
                m_WhitePos.remove(pos);
            else if (prev & Piece::Black)
                m_BlackPos.remove(pos);

            if (pnew & Piece::White)
                m_WhitePos.push_back(pos);
            else if (pnew & Piece::Black)
                m_BlackPos.push_back(pos);
        }
        struct Operation
        {
            Operation(Piece p, byte pos):Previous(p), Position(pos)
            { }
            const Piece Previous;
            const byte Position;
        };
        Piece *m_pData;
        std::list<byte> m_WhitePos;
        std::list<byte> m_BlackPos;
        std::list<Operation> m_Log;
    };
    __interface IPiececontroller
    {
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
        void MoveKing(Direction dirn, const Position& pos);

        bool IsValid(bool king, const Position& pos) const;
    };
    class WhitePiececontroller : public IPiececontroller
    {
    public:
        WhitePiececontroller(BoardBuilder *b) :m_pBuilder(b)
        { }
        bool CanJumpRight(const Position& pos) const
        {
            if (pos.Col > BOARD_SIZE - 3 || pos.Row > BOARD_SIZE - 3)
                return false;
            if (m_pBuilder->GetAt(pos.Row + 1, pos.Col + 1) & Piece::Black &&
                m_pBuilder->GetAt(pos.Row + 2, pos.Col + 2) == Piece::Empty)
                return true;
            return false;
        }
        void JumpRight(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, Piece::Empty);
            Position newpos(pos.Row + 2, pos.Col + 2);
            if (newpos.Row == BOARD_SIZE - 1)
                m_pBuilder->SetAt(newpos, (Piece::White | Piece::King));
            else
            {
                m_pBuilder->SetAt(newpos, mypiece);

                byte numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(Position(newpos.Row + 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(Position(newpos.Row + 2, newpos.Col - 2)) + 1;

                if (numrightjump != 0 || numrightjump != 0)
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
            if (pos.Col < 2 || pos.Row > BOARD_SIZE - 3)
                return false;
            if (m_pBuilder->GetAt(pos.Row + 1, pos.Col - 1) & Piece::Black &&
                m_pBuilder->GetAt(pos.Row + 2, pos.Col - 2) == Piece::Empty)
                return true;
            return false;
        }
        void JumpLeft(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, Piece::Empty);
            Position newpos(pos.Row + 2, pos.Col - 2);
            if (newpos.Row == BOARD_SIZE - 1)
                m_pBuilder->SetAt(newpos, (Piece::White | Piece::King));
            else
            {
                m_pBuilder->SetAt(newpos, mypiece);

                byte numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(Position(newpos.Row + 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(Position(newpos.Row + 2, newpos.Col - 2)) + 1;

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
                    m_pBuilder->GetAt(pos.Row + 1, pos.Col + 1) & Piece::Black &&
                    (m_pBuilder->GetAt(pos.Row + 2, pos.Col + 2) == Piece::Empty))
                where |= Direction::RightUp;
                if (pos.Col > 1 &&
                    m_pBuilder->GetAt(pos.Row + 1, pos.Col - 1) & Piece::Black &&
                    m_pBuilder->GetAt(pos.Row + 2, pos.Col - 2) == Piece::Empty)
                where |= Direction::LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < BOARD_SIZE - 2 &&
                    m_pBuilder->GetAt(pos.Row - 1, pos.Col + 1) & Piece::Black &&
                    (m_pBuilder->GetAt(pos.Row - 2, pos.Col + 2) == Piece::Empty))
                where |= Direction::RightDown;
                if (pos.Col > 1 &&
                    m_pBuilder->GetAt(pos.Row - 1, pos.Col - 1) & Piece::Black &&
                    m_pBuilder->GetAt(pos.Row - 2, pos.Col - 2) == Piece::Empty)
                where |= Direction::LeftDown;
            }
            return where;
        }
        void JumpKing(Direction dirn, const Position& pos) const
        {
            Position *pnewpos;
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);

            if (dirn == Direction::RightUp)
            {
                m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, Piece::Empty);
                pnewpos = new Position(pos.Row + 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftUp)
            {
                m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, Piece::Empty);
                pnewpos = new Position(pos.Row + 2, pos.Col - 2);
            }
            else if (dirn == Direction::RightDown)
            {
                m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, Piece::Empty);
                pnewpos = new Position(pos.Row - 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftDown)
            {
                m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, Piece::Empty);
                pnewpos = new Position(pos.Row - 2, pos.Col - 2);
            }
            else
                return;
            m_pBuilder->SetAt(*pnewpos, mypiece);


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
            if (pos.Col > BOARD_SIZE - 2 || pos.Row > BOARD_SIZE - 2)
                return false;
            if (m_pBuilder->GetAt(pos.Row + 1, pos.Col + 1) == Piece::Empty)
                return true;
            return false;
        }
        void MoveRight(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            if (pos.Row + 1 == BOARD_SIZE - 1)
                m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, (Piece::White | Piece::King));
            else
                m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, mypiece);
        }
        bool CanMoveLeft(const Position& pos) const
        {
            if (pos.Col < 1 || pos.Row > BOARD_SIZE - 2)
                return false;
            if (m_pBuilder->GetAt(pos.Row + 1, pos.Col - 1) == Piece::Empty)
                return true;
            return false;
        }
        void MoveLeft(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            if (pos.Row + 1 == BOARD_SIZE - 1)
                m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, (Piece::White | Piece::King));
            else
                m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, mypiece);
        }
        Direction CanMoveKing(const Position& pos) const
        {
            Direction where = Direction::None;
            if (pos.Row < BOARD_SIZE - 1)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBuilder->GetAt(pos.Row + 1, pos.Col + 1) == Piece::Empty)
                where |= Direction::RightUp;
                if (pos.Col > 0 &&
                    m_pBuilder->GetAt(pos.Row + 1, pos.Col - 1) == Piece::Empty)
                where |= Direction::LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBuilder->GetAt(pos.Row - 1, pos.Col + 1) == Piece::Empty)
                where |= Direction::RightDown;
                if (pos.Col > 0 &&
                    m_pBuilder->GetAt(pos.Row - 1, pos.Col - 1) == Piece::Empty)
                where |= Direction::LeftDown;
            }
            return where;
        }
        void MoveKing(Direction dirn, const Position& pos) const
        {
            if (dirn == Direction::None)
                return;
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            if ((dirn & Direction::RightUp) == Direction::RightUp)
                m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, mypiece);
            else if ((dirn & Direction::LeftUp) == Direction::LeftUp)
                m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, mypiece);
            else if ((dirn & Direction::RightDown) == Direction::RightDown)
                m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, mypiece);
            else if ((dirn & Direction::LeftDown) == Direction::LeftDown)
                m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, mypiece);
        }
        bool IsValid(bool king, const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            if (!(mypiece & Piece::White))
                return false;
            if (king && (mypiece & Piece::King))
                return true;
            if (!king && !(mypiece & Piece::King))
                return true;
            return false;
        }
    private:
        byte MaxJump(const Position& pos) const
        {
            byte rightjump = 0, leftjump = 0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(Position(pos.Row + 2, pos.Col + 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(Position(pos.Row + 2, pos.Col - 2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        byte MaxJumpKing(const Position& pos, Direction nothere) const
        {
            byte rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction::RightUp && (dirn & Direction::RightUp))
                rightUp = MaxJumpKing(Position(pos.Row + 2, pos.Col + 2), Direction::LeftDown) + 1;
            if (nothere != Direction::LeftUp && (dirn & Direction::LeftUp))
                leftUp = MaxJumpKing(Position(pos.Row + 2, pos.Col - 2), Direction::RightDown) + 1;
            if (nothere != Direction::RightDown && (dirn & Direction::RightDown))
                rightDown = MaxJumpKing(Position(pos.Row - 2, pos.Col + 2), Direction::LeftUp) + 1;
            if (nothere != Direction::LeftDown && (dirn & Direction::LeftDown))
                leftDown = MaxJumpKing(Position(pos.Row - 2, pos.Col - 2), Direction::RightUp) + 1;
            return Max({ rightUp, rightDown, leftUp, leftDown });
        }
        BoardBuilder *m_pBuilder;
    };
    class BlackPiececontroller : public IPiececontroller
    {
    public:
        BlackPiececontroller(BoardBuilder *b) :m_pBuilder(b)
        { }
        bool CanJumpRight(const Position& pos) const
        {
            if (pos.Col > BOARD_SIZE - 3 || pos.Row < 2)
                return false;
            if ((m_pBuilder->GetAt(pos.Row - 1, pos.Col + 1) & Piece::White) == Piece::White &&
                m_pBuilder->GetAt(pos.Row - 2, pos.Col + 2) == Piece::Empty)
                return true;
            return false;
        }
        void JumpRight(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, Piece::Empty);
            Position newpos(pos.Row - 2, pos.Col + 2);
            if (newpos.Row == 0)
                m_pBuilder->SetAt(newpos, (Piece::Black | Piece::King));
            else
            {
                m_pBuilder->SetAt(newpos, mypiece);

                byte numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(Position(newpos.Row - 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(Position(newpos.Row - 2, newpos.Col - 2)) + 1;

                if (numrightjump != 0 || numrightjump != 0)
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
            if ((m_pBuilder->GetAt(pos.Row - 1, pos.Col - 1) & Piece::White) == Piece::White &&
                m_pBuilder->GetAt(pos.Row - 2, pos.Col - 2) == Piece::Empty)
                return true;
            return false;
        }
        void JumpLeft(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, Piece::Empty);
            Position newpos(pos.Row - 2, pos.Col - 2);
            if (newpos.Row == 0)
                m_pBuilder->SetAt(newpos, (Piece::Black | Piece::King));
            else
            {
                m_pBuilder->SetAt(newpos, mypiece);

                byte numrightjump = 0, numleftjump = 0;
                if (CanJumpRight(newpos))
                    numrightjump = MaxJump(Position(newpos.Row - 2, newpos.Col + 2)) + 1;
                else if (CanJumpLeft(newpos))
                    numleftjump = MaxJump(Position(newpos.Row - 2, newpos.Col - 2)) + 1;

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
                    (m_pBuilder->GetAt(pos.Row + 1, pos.Col + 1) & Piece::White) == Piece::White &&
                    (m_pBuilder->GetAt(pos.Row + 2, pos.Col + 2) == Piece::Empty))
                where |= Direction::RightUp;
                if (pos.Col > 1 &&
                    (m_pBuilder->GetAt(pos.Row + 1, pos.Col - 1) & Piece::White) == Piece::White &&
                    (m_pBuilder->GetAt(pos.Row + 2, pos.Col - 2) == Piece::Empty))
                where |= Direction::LeftUp;
            }
            if (pos.Row > 1)
            {
                if (pos.Col < BOARD_SIZE - 2 &&
                    (m_pBuilder->GetAt(pos.Row - 1, pos.Col + 1) & Piece::White) == Piece::White &&
                    (m_pBuilder->GetAt(pos.Row - 2, pos.Col + 2) == Piece::Empty))
                where |= Direction::RightDown;
                if (pos.Col > 1 &&
                    (m_pBuilder->GetAt(pos.Row - 1, pos.Col - 1) & Piece::White) == Piece::White &&
                    (m_pBuilder->GetAt(pos.Row - 2, pos.Col - 2) == Piece::Empty))
                where |= Direction::LeftDown;
            }
            return where;
        }
        void JumpKing(Direction dirn, const Position& pos) const
        {
            Position *pnewpos;
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);

            if (dirn == Direction::RightUp)
            {
                m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, Piece::Empty);
                pnewpos = new Position(pos.Row + 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftUp)
            {
                m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, Piece::Empty);
                pnewpos = new Position(pos.Row + 2, pos.Col - 2);
            }
            else if (dirn == Direction::RightDown)
            {
                m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, Piece::Empty);
                pnewpos = new Position(pos.Row - 2, pos.Col + 2);
            }
            else if (dirn == Direction::LeftDown)
            {
                m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, Piece::Empty);
                pnewpos = new Position(pos.Row - 2, pos.Col - 2);
            }
            else
                return;
            m_pBuilder->SetAt(*pnewpos, mypiece);


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
            if (m_pBuilder->GetAt(pos.Row - 1, pos.Col + 1) == Piece::Empty)
                return true;
            return false;
        }
        void MoveRight(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            if (pos.Row - 1 == 0)
                m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, (Piece::Black | Piece::King));
            else
                m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, mypiece);
        }
        bool CanMoveLeft(const Position& pos) const
        {
            if (pos.Col < 1 || pos.Row < 1)
                return false;
            if (m_pBuilder->GetAt(pos.Row - 1, pos.Col - 1) == Piece::Empty)
                return true;
            return false;
        }
        void MoveLeft(const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            if (pos.Row - 1 == 0)
                m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, (Piece::Black | Piece::King));
            else
                m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, mypiece);
        }

        Direction CanMoveKing(const Position& pos) const
        {
            Direction where = Direction::None;
            if (pos.Row < BOARD_SIZE - 1)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBuilder->GetAt(pos.Row + 1, pos.Col + 1) == Piece::Empty)
                where |= Direction::RightUp;
                if (pos.Col > 0 &&
                    m_pBuilder->GetAt(pos.Row + 1, pos.Col - 1) == Piece::Empty)
                where |= Direction::LeftUp;
            }
            if (pos.Row > 0)
            {
                if (pos.Col < BOARD_SIZE - 1 &&
                    m_pBuilder->GetAt(pos.Row - 1, pos.Col + 1) == Piece::Empty)
                where |= Direction::RightDown;
                if (pos.Col > 0 &&
                    m_pBuilder->GetAt(pos.Row - 1, pos.Col - 1) == Piece::Empty)
                where |= Direction::LeftDown;
            }
            return where;
        }
        void MoveKing(Direction dirn, const Position& pos) const
        {
            if (dirn == Direction::None)
                return;
            Piece mypiece = m_pBuilder->GetAt(pos);
            m_pBuilder->SetAt(pos, Piece::Empty);
            if ((dirn & Direction::RightUp) == Direction::RightUp)
                m_pBuilder->SetAt(pos.Row + 1, pos.Col + 1, mypiece);
            else if ((dirn & Direction::LeftUp) == Direction::LeftUp)
                m_pBuilder->SetAt(pos.Row + 1, pos.Col - 1, mypiece);
            else if ((dirn & Direction::RightDown) == Direction::RightDown)
                m_pBuilder->SetAt(pos.Row - 1, pos.Col + 1, mypiece);
            else if ((dirn & Direction::LeftDown) == Direction::LeftDown)
                m_pBuilder->SetAt(pos.Row - 1, pos.Col - 1, mypiece);
        }

        bool IsValid(bool king, const Position& pos) const
        {
            Piece mypiece = m_pBuilder->GetAt(pos);
            if ((mypiece & Piece::Black) != Piece::Black)
                return false;
            if (king && (mypiece & Piece::King) == Piece::King)
                return true;
            if (!king && (mypiece & Piece::King) == 0)
                return true;
            return false;
        }
    private:
        byte MaxJump(const Position& pos) const
        {
            byte rightjump = 0, leftjump = 0;
            if (CanJumpRight(pos))
                rightjump = MaxJump(Position(pos.Row - 2, pos.Col + 2)) + 1;
            if (CanJumpLeft(pos))
                leftjump = MaxJump(Position(pos.Row - 2, pos.Col - 2)) + 1;
            return rightjump > leftjump ? rightjump : leftjump;
        }
        byte MaxJumpKing(const Position& pos, Direction nothere) const
        {
            byte rightUp = 0, leftUp = 0, rightDown = 0, leftDown = 0;
            Direction dirn = CanJumpKing(pos);
            if (nothere != Direction::RightUp && (dirn & Direction::RightUp))
                rightUp = MaxJumpKing(Position(pos.Row + 2, pos.Col + 2), Direction::LeftDown) + 1;
            if (nothere != Direction::LeftUp && (dirn & Direction::LeftUp))
                leftUp = MaxJumpKing(Position(pos.Row + 2, pos.Col - 2), Direction::RightDown) + 1;
            if (nothere != Direction::RightDown && (dirn & Direction::RightDown))
                rightDown = MaxJumpKing(Position(pos.Row - 2, pos.Col + 2), Direction::LeftUp) + 1;
            if (nothere != Direction::LeftDown && (dirn & Direction::LeftDown))
                leftDown = MaxJumpKing(Position(pos.Row - 2, pos.Col - 2), Direction::RightUp) + 1;
            return Max({ rightUp, rightDown, leftUp, leftDown });
        }
        BoardBuilder *m_pBuilder;
    };
}