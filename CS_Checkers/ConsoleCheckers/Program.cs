using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Checkers
{
    class ConsoleProgram
    {
        static void Main(string[] args)
        {
            Gameplay game = new Gameplay();

            bool colorSet;
            do
            {
                colorSet = true;
                Console.WriteLine("Enter player color:");
                string color = Console.ReadLine();
                if (color.ToLower() == "white")
                    game.WhitePlayer = true;
                else if (color.ToLower() == "black")
                    game.WhitePlayer = false;
                else
                    colorSet = false;
            } while (!colorSet);

            bool cont;
            Console.WriteLine("Direction:\nru = right up\nrd = right down\nlu = left up\nld = left down\n");
            if (!game.WhitePlayer)
            {
                Aux.Print(game);
                Console.WriteLine("\nAI thinking . . . . . . . .\n");
                game.AITurn();
            }
            do
            {
                Aux.Print(game);

                bool validPlayerMove;
                do
                {
                    Console.WriteLine("Enter [row col direction]");
                    string[] inputs = Console.ReadLine().Split(' ');
                    if (inputs.Length < 3)
                        return;
                    int row = Convert.ToInt32(inputs[0]);
                    int col = Convert.ToInt32(inputs[1]);
                    Direction dirn;
                    switch (inputs[2])
                    {
                        case "ru":
                            dirn = Direction.RightUp;
                            break;
                        case "rd":
                            dirn = Direction.RightDown;
                            break;
                        case "lu":
                            dirn = Direction.LeftUp;
                            break;
                        case "ld":
                            dirn = Direction.LeftDown;
                            break;
                        default:
                            dirn = Direction.None;
                            break;
                    }
                    validPlayerMove = game.PlayerTurn(row, col, dirn);
                    if (!validPlayerMove)
                        Console.WriteLine("Invalid move! Try again.");
                } while (!validPlayerMove);

                Aux.Print(game);
                Console.WriteLine("\nAI thinking . . . . . . . .\n");
                cont = game.AITurn();
            } while (cont);

            Console.ReadKey();
        }
    }
}
