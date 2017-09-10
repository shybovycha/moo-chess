<?php
	define('DB_NAME', 'moo_chess');
	define('DB_HOST', 'localhost');
	define('DB_USER', 'root');
	define('DB_PASSWORD', 'password');

	function __hardcoreLog($msg)
	{
		$f = fopen('general_error.log', 'a');
		fwrite($f, date('d.m.Y H:i:s') . ' :: ' . $msg . "\n\n");
		fclose($f);
	}

	function __exceptionHandler($exception)
	{
		__hardcoreLog("Exception found :: $exception");
	}

	function __errorHandler($errno, $errstr, $errfile, $errline)
	{
		__hardcoreLog("Error found :: $errstr (#{$errno} at {$errfile} : {$errline})");
	}

	set_error_handler('__errorHandler');
	set_exception_handler('__exceptionHandler');

	class Application
	{
		private $db_link;

		function __construct()
		{
			$this->loggingEnabled = FALSE;
			$this->db_link = mysql_connect(DB_HOST, DB_USER, DB_PASSWORD);
			self::__hardcoreDBLog();
			mysql_select_db(DB_NAME, $this->db_link);
			self::__hardcoreDBLog();
		}

		final private static function __hardcoreDBLog($query = NULL)
		{
			if (isset($query))
				$q = 'EXECUTING :: "' . $query . '"'; else
					$q = '';

			if (mysql_errno() > 0)
			{
				$f = fopen('mysql_error.log', 'a');
				fwrite($f, date('d.m.Y H:i:s') . ' :: ' . $q . __FILE__ . ' :: ' . __LINE__ . ' :: ' . mysql_error() . "\n\n");
				fclose($f);

				die(mysql_error());
			}
		}

		final private static function __silentDump()
		{
			ob_start();
			$x = func_get_args();
			var_dump($x);
			$res = ob_get_contents();
			ob_end_clean();

			return $res;
		}

		final private function __query($query = NULL)
		{
			if (!isset($query))
			{
				return NULL;
			}

			$res = mysql_query($query);

			self::__hardcoreDBLog($query);

			if (is_resource($res))
			{
				$ret = array();

				while ($row = mysql_fetch_assoc($res))
				{
					$ret[] = $row;
				}
			} else
			{
				$ret = $res;
			}

			return $ret;
		}

		final public function __route()
		{
			$actions = get_class_methods($this);
			$cnt = 0;

			foreach ($actions as $i)
			{
				if (isset($_GET[$i]))
				{
					$this->$i();
					$cnt++;
				}
			}

			if ($cnt <= 0)
			{
				$this->__index();
			}
		}

		final private function __getParam($key = NULL)
		{
			if (!isset($key) || !isset($_POST[$key]))
				return NULL; else
			if (isset($_POST[$key]))
				return $_POST[$key];
		}

		final public function __index()
		{
			echo '<h1>Hello, World!</h1>';
		}

		function __log($message = NULL)
		{
			if (!isset($message) || !isset($this->loggingEnabled) || !$this->loggingEnabled)
				return FALSE;

			$date = date('d.m.Y H:i:s');
			$message = mysql_real_escape_string($message, $this->db_link);

			$this->__query("INSERT INTO logs (date, message) VALUES ('$date', '$message');");
		}

		private function moo($gid, $board, $from, $to)
		{
			$c1 = -1; $r1 = -1; $c2 = -1; $r2 = -1;
			sscanf($from, "%c%d", $c1, $r1);
			sscanf($to, "%c%d", $c2, $r2);

			if (!isset($board))
			{
				$this->__log("Board is not set!!!");

				return 0;
			}

			$n1 = $board[$r1][$c1]; $n2 = $board[$r2][$c2];

			if ($n1 <= 0 || $r1 > 8 || $r1 < 1 || $c1 > 'h' || $c1 < 'a' || $r2 > 8 || $r2 < 1 || $c2 > 'h' || $c2 < 'a')
			{
				$this->__log("Trying to make a very invalid move: {$c1}{$r1} -> {$c2}{$r2} (" . $this->__silentDump($board) .")");

				return 0;
			}

			// Move deltas: deltaX and deltaY are deltaColumn and deltaRow, respectively
			$dx = (ord($c2) - ord('a')) - (ord($c1) - ord('a'));
			$dy = $r2 - $r1;

			// Move from [self] to [self] is not allowed
			if ($dx == 0 && $dy == 0)
			{
				$this->__log("Trying to make a move from {$c1}{$r1} to {$c2}{$r2} :: moves like this are not allowed");

				return 0;
			}

			// Knight is the easiest figure for implementation
			if ($n1 == 3 || $n1 == 13)
			{
				if ((abs($dx) == 1 && abs($dy) == 2) || (abs($dx) == 2 && abs($dy) == 1))
				{
					if (($n1 > 10 && $n2 > 10) || ($n1 > 0 && $n2 > 0 && $n2 < 10 && $n1 < 10))
					{
						$this->__log('capturing own figures is not allowed');

						return 0;
					}
				} else
				{
					$this->__log('this is not a valid move for a knight');

					return 0;
				}
			} else
			// King is also easy to implement
			if ($n1 == 6 || $n1 == 16)
			{
				if (abs($dy) > 1)
				{
					$this->__log('invalid move for a king');

					return 0;
				}

				if (abs($dx) > 1)
				{
					if ($from == 'e8')
					{
						$this->__log('CASTLING FROM E8');

						if ($to != 'g8' && $to != 'c8')
						{
							$this->__log("invalid move =(");

							return 0;
						} else
						if ($to == 'g8')
						{
							$rook = 'h8';

							$castlingRes = $this->__query("SELECT * FROM legends WHERE game_id = $gid AND ( move_from = '{$from}' OR move_to = '{$from}' OR move_from = '{$rook}' OR move_to = '{$rook}' );");

							$this->__log($this->__silentDump(  "SELECT * FROM legends WHERE game_id = $gid AND ( move_from = '{$from}' OR move_to = '{$from}' OR move_from = '{$rook}' OR move_to = '{$rook}' );"  ));

							if ((is_array($castlingRes) && count($castlingRes) > 0) || ($board['8']['f'] > 0 || $board['8']['g'] > 0))
							{
								$this->__log("invalid move ...");

								return 0;
							}

							$this->__log('CASTLING TO G8');

							return 4;
						} else
						if ($to == 'c8')
						{
							$rook = 'a8';

							$castlingRes = $this->__query("SELECT * FROM legends WHERE game_id = $gid AND ( move_from = '{$from}' OR move_to = '{$from}' OR move_from = '{$rook}' OR move_to = '{$rook}' );");

							if ((is_array($castlingRes) && count($castlingRes) > 0) || ($board['8']['b'] > 0 || $board['8']['c'] > 0 || $board['8']['d'] > 0))
							{
								$this->__log("invalid move ...");

								return 0;
							}

							$this->__log('CASTLING TO C8');

							return 4;
						}
					} else
					if ($from == 'e1')
					{
						$this->__log('CASTLING FROM E1');

						if ($to != 'c1' && $to != 'g1')
						{
							$this->__log("invalid move =(");

							return 0;
						} else
						if ($to == 'g1')
						{
							$rook = 'h1';

							$castlingRes = $this->__query("SELECT * FROM legends WHERE game_id = $gid AND ( move_from = '{$from}' OR move_to = '{$from}' OR move_from = '{$rook}' OR move_to = '{$rook}' );");

							if ((is_array($castlingRes) && count($castlingRes) > 0) || ($board['1']['f'] > 0 || $board['1']['g'] > 0))
							{
								$this->__log("invalid move ...");

								return 0;
							}

							$this->__log('CASTLING TO G1');

							return 4;
						} else
						if ($to == 'c1')
						{
							$rook = 'a1';

							$castlingRes = $this->__query("SELECT * FROM legends WHERE game_id = $gid AND ( move_from = '{$from}' OR move_to = '{$from}' OR move_from = '{$rook}' OR move_to = '{$rook}' );");

							if ((is_array($castlingRes) && count($castlingRes) > 0) || ($board['1']['b'] > 0 || $board['1']['c'] > 0 || $board['1']['d'] > 0))
							{
								$this->__log("invalid move ...");

								return 0;
							}

							$this->__log('CASTLING TO C1');

							return 4;
						}
					}

					$this->__log('invalid move for a king');

					return 0;
				}

				if (($n1 > 0 && $n1 < 10 && $n2 > 0 && $n2 < 10) || ($n1 > 10 && $n2 > 10))
				{
					$n2 = ($n2 > 10) ? $n2 - 10 : $n2;

					$this->__log("capturing own figures is not allowed ({$figures[$n2]})");

					return 0;
				}
			} else
			// White pawn is easy enough to implement
			if ($n1 == 1)
			{
				$dy = $dy * -1;

				if ($r2 > $r1)
				{
					$this->__log('moving backwards is not allowed for pawn');

					return 0;
				}

				if ($n2 > 0 && $n2 < 10)
				{
					$this->__log('capturing own figures is not allowed');

					return 0;
				} else
				if ($n2 > 10)
				{
					if ($dy == 1 && abs($dx) == 1)
					{
					} else
					{
						$this->__log('capturing in such manner is not allowed');

						return 0;
					}
				} else
				if ($n2 == 0)
				{
					if (($r1 == 7 && $dy <= 2 && $dx == 0) || ($dx == 0 && $dy == 1 && $r1 < 7))
					{
					} else
					{
						$this->__log('moving in such manner is not allowed for pawn');

						return 0;
					}
				} else
				{
					$this->__log('strange move for a pawn...');

					return 0;
				}
			} else
			// Black pawn is just a clone of white one except of a few constants
			if ($n1 == 11)
			{
				if ($r2 < $r1)
				{
					$this->__log('moving backwards is not allowed for pawn');

					return 0;
				}

				if ($n2 > 10)
				{
					$this->__log('capturing own figures is not allowed');

					return 0;
				} else
				if ($n2 < 10 && $n2 > 0)
				{
					if ($dy == 1 && abs($dx) == 1)
					{
					} else
					{
						$this->__log('capturing in such manner is not allowed');

						return 0;
					}
				} else
				if ($n2 == 0)
				{
					if (($r1 == 2 && $dy <= 2 && $dx == 0) || ($dx == 0 && $dy == 1 && $r1 > 2))
					{
					} else
					{
						$this->__log('moving in such manner is not allowed for pawn');

						return 0;
					}
				} else
				{
					$this->__log('strange move for a pawn...');

					return 0;
				}
			} else
			// Rook is not so hard to implement
			if ($n1 == 2 || $n1 == 12)
			{
				// exclude diagonal moves
				if ($dx != 0 && $dy != 0)
				{
					$this->__log('rook could move only straight');

					return 0;
				}

				$a_row = $r1; $b_row = $r2;
				$a_col = ord($c1); $b_col = ord($c2);
				$row_delta = intval(($b_row - $a_row) / abs($b_row - $a_row));
				$col_delta = intval(($b_col - $a_col) / abs($b_col - $a_col));

				while ($a_row != $b_row || $a_col != $b_col)
				{
					$a_row += $row_delta;
					$a_col += $col_delta;

					if (($a_row != $b_row || $a_col != $b_col) && ($a_row > 8 || $a_row < 1 || $a_col > ord('h') || $a_col < ord('a') || $board[$a_row][chr($a_col)] > 0))
					{
						$this->__log('invalid path for rook');

						return 0;
					}
				}

				if ($a_row == $b_row && $a_col == $b_col)
				{
					$this->__log('well done');

					return 1;
				} else
				{
					$this->__log('dunno why but this move is invalid for a rook');

					return 0;
				}
			} else
			// bishop is easier than a rook to implement
			if ($n1 == 4 || $n1 == 14)
			{
				// exclude straight moves
				if (abs($dx) != abs($dy) || $dx == 0 || $dy == 0)
				{
					$this->__log('bishop could move diagonally only');

					return 0;
				}

				if (($n1 == 4 && $n2 > 0 && $n2 < 10) || ($n1 == 14 && $n2 > 10))
				{
					$this->__log('could not capture own figures');

					return 0;
				}

				$a_row = $r1; $b_row = $r2;
				$a_col = ord($c1); $b_col = ord($c2);
				$row_delta = intval(($b_row - $a_row) / abs($b_row - $a_row));
				$col_delta = intval(($b_col - $a_col) / abs($b_col - $a_col));

				while ($a_row != $b_row || $a_col != $b_col)
				{
					$a_row += $row_delta;
					$a_col += $col_delta;

					if (($a_row != $b_row || $a_col != $b_col) && ($a_row > 8 || $a_row < 1 || $a_col > ord('h') || $a_col < ord('a') || $board[$a_row][chr($a_col)] > 0))
					{
						$this->__log('invalid path for bishop');

						return 0;
					}
				}

				if ($a_row == $b_row && $a_col == $b_col)
				{
					return 1;
				} else
				{
					$this->__log('Don\'t know why, but this is invalid move for a rook');

					return 0;
				}
			}  else
			// queen is just a compilation of rook and bishop
			if ($n1 == 5 || $n1 == 15)
			{
				if (!((abs($dx) != abs($dy) && (($dx == 0 && $dy != 0) || ($dx != 0 && $dy == 0))) || (abs($dx) == abs($dy) && $dx != 0)))
				{
					$this->__log('queen could move diagonally or straight only');

					return 0;
				}

				if (($n1 == 5 && $n2 > 0 && $n2 < 10) || ($n1 == 15 && $n2 > 10))
				{
					$this->__log('could not capture own figures');

					return 0;
				}

				$a_row = $r1; $b_row = $r2;
				$a_col = ord($c1); $b_col = ord($c2);
				$row_delta = intval(($b_row - $a_row) / abs($b_row - $a_row));
				$col_delta = intval(($b_col - $a_col) / abs($b_col - $a_col));

				while ($a_row != $b_row || $a_col != $b_col)
				{
					$a_row += $row_delta;
					$a_col += $col_delta;

					$c = chr($a_col);
					$r = $a_row;

					$this->__log('testing' . $this->__silentDump("$r $c {$board[$a_row][$c]}"));

					if (($a_row != $b_row || $a_col != $b_col) && ($a_row > 8 || $a_row < 1 || $a_col > ord('h') || $a_col < ord('a') || $board[$a_row][chr($a_col)] > 0))
					{
						$this->__log('invalid path for a queen');

						return 0;
					}
				}

				if ($a_row == $b_row && $a_col == $b_col)
				{
					return 1;
				} else
				{
					$this->__log('donno know why, but this is invalid move for a queen');

					return 0;
				}
			}

			return 1;
		}

		private function validate_position($gid = NULL, $from = NULL, $to = NULL)
		{
			/*
			 * Response reference:
			 *
			 * 0 - invalid move
			 * 1 - usual move - move figure from [from] cell to the [to] one
			 * 4 - castling (swap rook and king for 2 cells)
			 * 8 - en passant (remove neighbour pawn and place player's one as it captured other when it moves one cell less)
			 * 16 - promote pawn to queen
			 * 17 - promote pawn to bishop
			 * 18 - promote pawn to knight
			 * 19 - promote pawn to rook
			 * 32 - check to whites
			 * 64 - checkmate
			 */

			if (!isset($gid) || !isset($from) || !isset($to))
			{
				$this->__log('invalid validate_position params: ' . self::__silentDump(array('game_id' => $gid, 'from' => $from, 'to' => $to)));

				return 0;
			}

			$board_res = $this->__query("SELECT * FROM boards WHERE game_id = '{$gid}' LIMIT 1;");

			if (!is_array($board_res) || count($board_res) <= 0)
			{
				$this->__log("something is wrong with game #${gid}");

				return 0;
			}

			$board = unserialize($board_res[0]['data']);

			$c1 = -1; $r1 = -1; $c2 = -1; $r2 = -1;
			sscanf($from, "%c%d", $c1, $r1);
			sscanf($to, "%c%d", $c2, $r2);

			$n1 = $board[$r1][$c1]; $n2 = $board[$r2][$c2];

			if ($n1 <= 0 || $r1 > 8 || $r1 < 1 || $c1 > 'h' || $c1 < 'a' || $r2 > 8 || $r2 < 1 || $c2 > 'h' || $c2 < 'a')
			{
				$this->__log("Trying to make a very invalid move: {$c1}{$r1} -> {$c2}{$r2} (" . $this->__silentDump($board) .")");

				return 0;
			}

			$figures = array(
				0 => 'empty cell',
				1 => 'pawn', // *
				2 => 'rook', // *
				3 => 'knight', // *
				4 => 'bishop', // *
				5 => 'queen', // *
				6 => 'king', // 50-50
			);

			$validation_res = $this->moo($gid, $board, $from, $to);

			if ($validation_res <= 0)
			{
				return 0;
			} else
			if ($validation_res == 1)
			{
				$board[$r2][$c2] = $board[$r1][$c1];
				$board[$r1][$c1] = 0;
			} else
			if ($validation_res == 4)
			{
				// white
				if ($from == 'e8')
				{
					if ($to == 'g8')
					{
						$board['e']['8'] = 0;
						$board['h']['8'] = 0;
						$board['g']['8'] = 6;
						$board['8']['f'] = 2;
					} else
					if ($to == 'c8')
					{
						$board['8']['e'] = 0;
						$board['8']['a'] = 0;
						$board['8']['c'] = 6;
						$board['8']['d'] = 2;
					}
				} else
				// black
				if ($from == 'e1')
				{
					if ($to == 'g1')
					{
						$board['1']['e'] = 0;
						$board['1']['h'] = 0;
						$board['1']['g'] = 16;
						$board['1']['f'] = 12;
					} else
					if ($to == 'c1')
					{
						$board['1']['e'] = 0;
						$board['1']['h'] = 0;
						$board['1']['g'] = 16;
						$board['1']['f'] = 12;
					}
				}
			}

			//$this->__log('Board :: ' . $this->__silentDump($board));

			$board = serialize($board);

			$res = $this->__query("UPDATE boards SET data = '{$board}' WHERE game_id = {$gid};");

			if (!$res)
			{
				$this->__log('Error while executing :: ' . "UPDATE boards SET data = '{$board}' WHERE game_id = {$gid};");

				return 0;
			}

			$this->__log("moving {$figures[($n1 > 10) ? $n1 - 10 : $n1]} from {$from} to {$to} ({$figures[($n2 > 10) ? $n2 - 10 : $n2]}) => $validation_res");

			return $validation_res;
		}

		public function find_side()
		{
			$res = $this->__query('SELECT IF((SELECT COUNT(*) FROM games WHERE client_a IS NOT NULL) > (SELECT COUNT(*) FROM games WHERE client_b IS NOT NULL), \'black\', \'white\') AS side');

			$side = NULL;

			if (!is_array($res) || count($res) <= 0)
			{
				$this->__log('something went wrong while trying to balance players');

				$side = (rand(-100, 100) > 0) ? 'white' : 'black';
			} else
			{
				$side = $res[0]['side'];
			}

			if ($side == 'white')
				echo '2'; else
					echo '7';
		}

		public function find_opponent()
		{
			$side = array('search' => NULL, 'opponent' => NULL, 'value' => md5(date('H:m:s') . rand(0, 100)));

			if ($this->__getParam('side') == 'white')
			{
				$side['search'] = 'client_b';
				$side['opponent'] = 'client_a';
			} else
			if ($this->__getParam('side') == 'black')
			{
				$side['search'] = 'client_a';
				$side['opponent'] = 'client_b';
			} else
			{
				$this->__log('fake side is given: ' . $this->__getParam('side'));
				$this->__log('post params: ' . self::__silentDump($_POST));

				return FALSE;
			}

			/*
			 * Board reference:
			 *
			 * 0 - empty cell
			 * 1 - pawn
			 * 2 - rook
			 * 3 - knight
			 * 4 - bishop
			 * 5 - queen
			 * 6 - king
			 *
			 * 1 .. 6 - whites
			 * 11 .. 16 - blacks
			 */

			$default_board = array(
				'1' => array('a' => 12, 'b' => 13, 'c' => 14, 'd' => 15, 'e' => 16, 'f' => 14, 'g' => 13, 'h' => 12),
				'2' => array('a' => 11, 'b' => 11, 'c' => 11, 'd' => 11, 'e' => 11, 'f' => 11, 'g' => 11, 'h' => 11),
				'3' => array('a' => 0, 'b' => 0, 'c' => 0, 'd' => 0, 'e' => 0, 'f' => 0, 'g' => 0, 'h' => 0),
				'4' => array('a' => 0, 'b' => 0, 'c' => 0, 'd' => 0, 'e' => 0, 'f' => 0, 'g' => 0, 'h' => 0),
				'5' => array('a' => 0, 'b' => 0, 'c' => 0, 'd' => 0, 'e' => 0, 'f' => 0, 'g' => 0, 'h' => 0),
				'6' => array('a' => 0, 'b' => 0, 'c' => 0, 'd' => 0, 'e' => 0, 'f' => 0, 'g' => 0, 'h' => 0),
				'7' => array('a' => 1, 'b' => 1, 'c' => 1, 'd' => 1, 'e' => 1, 'f' => 1, 'g' => 1, 'h' => 1),
				'8' => array('a' => 2, 'b' => 3, 'c' => 4, 'd' => 5, 'e' => 6, 'f' => 4, 'g' => 3, 'h' => 2),
			);

			$found = '?';

			$this->__log("searching {$side['search']} opponent for a game with client({$side['opponent']})");

			$res = $this->__query("SELECT * FROM games WHERE {$side['opponent']} IS NULL AND {$side['search']} IS NOT NULL ORDER BY RAND() LIMIT 1;");

			if (is_array($res) && count($res) == 1)
			{
				$gid = $res[0]['id'];

				$data = serialize($default_board);

				$this->__log("found opponent");

				$this->__query("UPDATE games SET ${side['opponent']} = '${side['value']}' WHERE id = $gid;");
				$this->__query("INSERT INTO boards (game_id, data) VALUES ($gid, '$data');");

				$this->__log("game #${gid} found for client #${side['value']}");

				$found = '!';
			} else
			if (is_array($res) && count($res) <= 0)
			{
				$data = serialize($default_board);

				$this->__log("opponent not found. creating a game");

				$query = "INSERT INTO games (${side['opponent']}) VALUES ('${side['value']}');";

				$this->__query($query);

				$this->__log("game created for client #${side['value']}");
			}

			echo $found . $side['value'];
		}

		public function is_game_started()
		{
			$client_id = trim($this->__getParam('client'));

			$res = $this->__query("SELECT * FROM games WHERE ((client_a = '$client_id' AND client_b IS NOT NULL) OR (client_b = '$client_id' AND client_a IS NOT NULL));");

			/*
			 * Response reference:
			 *
			 * 0 - game is not started
			 * 1 - game started
			 */

			$client_id = trim($client_id);

			if (is_array($res) && count($res) > 0)
			{
				$this->__log("SUCC ::: SELECT * FROM games WHERE ((client_a = '$client_id' AND client_b IS NOT NULL) OR (client_b = '$client_id' AND client_a IS NOT NULL));");

				echo '1';
			} else
			{
				$this->__log("FAIL ::: SELECT * FROM games WHERE ((client_a = '$client_id' AND client_b IS NOT NULL) OR (client_b = '$client_id' AND client_a IS NOT NULL));" . $this->__silentDump($res));

				echo '0';
			}
		}

		public function update_client()
		{
			/*
			 * Response reference:
			 *
			 * 0 - no new moves or no response
			 * 1 - opponent's turn
			 * 2 - user's turn
			 * 3 - opponent's check
			 * 4 - opponent's checkmate
			 * 5 - user's ckeck
			 * 6 - user's checkmate
			 * 7 - opponent's castling
			 * 12 - set up game with whites on the top
			 * 17 - set up game with whites on the bottom
			 */

			$client = trim($this->__getParam('client'));

			$res = $this->__query("SELECT * FROM games WHERE ((client_a = '$client' AND client_b IS NULL) OR (client_b = '$client' AND client_a IS NULL));");

			if (is_array($res) && count($res) > 0)
			{
				$res = $res[0];

				if ($res['client_a'] == $client && $res['client_b'] == '')
				{
					echo '12';

					$this->__log("game [${res['client_a']}] VS [${res['client_b']}] starts!");

					return TRUE;
				} else
				if ($res['client_b'] == $client && $res['client_a'] == '')
				{
					echo '17';

					$this->__log("game [${res['client_a']}] VS [${res['client_b']}] starts!");

					return TRUE;
				} else
				{
					$this->__log("could not start game for client #${client}");
				}
			}

			$res = $this->__query("SELECT * FROM games WHERE ((client_a = '$client' AND client_b IS NOT NULL) OR (client_b = '$client' AND client_a IS NOT NULL)) LIMIT 1;");

			if (!is_array($res) || count($res) < 1)
			{
				echo '0';

				$this->__log("could not find game for client #${client} ::: " . "SELECT * FROM games WHERE ((client_a = '$client' AND client_b IS NOT NULL) OR (client_b = '$client' AND client_a IS NOT NULL)) LIMIT 1;");

				return FALSE;
			}

			$gid = $res[0]['id'];

			if ($res[0]['client_a'] == $client)
				$side = 'white'; else
			if ($res[0]['client_b'] == $client)
				$side = 'black';

			$hist_res = $this->__query("SELECT * FROM legends WHERE game_id = $gid ORDER BY date DESC LIMIT 1;");

			if (is_array($hist_res) && count($hist_res) == 1)
			{
				$hist_res = $hist_res[0];
				$hist_castling = FALSE;

				if ($hist_res['move_from'] == 'e1' || $hist_res['move_from'] == 'e8')
				{
					$this->__log('???CASTLING DETECTED???');

					$from = $hist_res['move_from'];
					$date = $hist_res['date'];

					$_q = "SELECT * FROM legends WHERE game_id = $gid AND client <> '$client' AND move_from = '$from' AND date < '$date';";
					$castling_res = $this->__query($_q);

					if ((is_array($castling_res) && count($castling_res) <= 0) || (!is_array($castling_res)))
					{
						$hist_castling = TRUE;

						$this->__log('!!!CASTLING DETECTED!!!');
					} else
					{
						$this->__log("FAIL ::: {$_q}");
					}
				}

				if ($side == 'black')
				{
					$c1 = '';
					$d1 = -1;
					$c2 = '';
					$d2 = -1;

					sscanf($hist_res['move_from'], '%c%d', $c1, $d1);
					sscanf($hist_res['move_to'], '%c%d', $c2, $d2);

					$c1 = chr(ord('h') - ord($c1) + ord('a'));
					$d1 = 8 - $d1 + 1;
					$c2 = chr(ord('h') - ord($c2) + ord('a'));
					$d2 = 8 - $d2 + 1;

					$hist_res['move_from'] = "$c1$d1";
					$hist_res['move_to'] = "$c2$d2";
				}

				if ($hist_res['client'] != $client)
				{
					if (!$hist_castling)
						echo '2'; else
							echo '7';

					echo ' ' . $hist_res['move_from'] . ' ' . $hist_res['move_to'];
				} else
				{
					echo '1';
				}

				return TRUE;
			} else
			if (is_array($hist_res) && count($hist_res) == 0)
			{
				if ($side == 'black')
				{
					echo '1';
				} else
				{
					echo '0';
				}

				return TRUE;
			}

			$this->__log("something is wrong with client #${client}");

			echo '0';
		}

		public function  validate_move()
		{
			/*
			 * Response reference:
			 *
			 * 0 - invalid move
			 * 1 - usual move - move figure from [from] cell to the [to] one
			 * 4 - castling (swap rook and king for 2 cells)
			 * 8 - en passant (remove neighbour pawn and place player's one as it captured other when it moves one cell less)
			 * 16 - promote pawn to queen
			 * 17 - promote pawn to bishop
			 * 18 - promote pawn to knight
			 * 19 - promote pawn to rook
			 * 32 - check to whites
			 * 64 - checkmate
			 */

			$client = $this->__getParam('client');
			$from = $this->__getParam('from');
			$to = $this->__getParam('to');
			$move_pattern = '/^[a-h][1-8]$/';

			if (!preg_match($move_pattern, $from) || !preg_match($move_pattern, $to))
			{
				echo '0';

				$this->__log("something is wrong with client #${client} (from = $from && to = $to)");

				return FALSE;
			}

			$game_res = $this->__query("SELECT * FROM games WHERE ((client_a = '$client' AND client_b IS NOT NULL) OR (client_b = '$client' AND client_a IS NOT NULL));");

			if (is_array($game_res) && count($game_res) == 1)
			{
				$game_res = $game_res[0];
				$gid = $game_res['id'];

				if ($game_res['client_a'] == $client)
					$side = 'white'; else
				if ($game_res['client_b'] == $client)
				{
					$side = 'black';

					$c = '';
					$d = -1;

					sscanf($from, '%c%d', $c, $d);

					$c = chr(ord('h') - ord($c) + ord('a'));
					$d = 8 - $d + 1;

					$from = "$c$d";

					$c = '';
					$d = -1;

					sscanf($to, '%c%d', $c, $d);

					$c = chr(ord('h') - ord($c) + ord('a'));
					$d = 8 - $d + 1;

					$to = "$c$d";
				}

				$hist_res = $this->__query("SELECT * FROM legends WHERE game_id = $gid ORDER BY date DESC LIMIT 1;");

				if (is_array($hist_res) && count($hist_res) == 1)
				{
					$hist_res = $hist_res[0];

					if ($hist_res['client'] != $client)
					{
						$validation_res = intval($this->validate_position($gid, $from, $to));

						if ($validation_res)
						{
							echo $validation_res;

							$date = date('d.m.Y H:i:s');
							$this->__query("INSERT INTO legends (game_id, client, date, move_from, move_to) VALUES ($gid, '$client', '$date', '$from', '$to');");

							$this->__log("valid move from $from to $to ($validation_res)");
						} else
						{
							echo '0';

							$this->__log("INVALID move from $from to $to");
						}
					} else
					{
						echo '0';

						$this->__log("INVALID move from $from to $to");
					}

					return TRUE;
				} else
				{
					if ($side == 'white')
					{
						$validation_res = intval($this->validate_position($gid, $from, $to));

						if ($validation_res)
						{
							echo $validation_res;

							$date = date('d.m.Y H:i:s');
							$this->__query("INSERT INTO legends (game_id, client, date, move_from, move_to) VALUES ($gid, '$client', '$date', '$from', '$to');");

							$this->__log("valid FIRST move from $from to $to ($validation_res)");
						} else
						{
							echo '0';

							$this->__log("INVALID FIRST move from $from to $to");
						}
					} else
					{
						echo '0';
					}

					return TRUE;
				}
			}

			$this->__log("something is wrong with client #${client} (could not find valid game)");

			echo '0';

			return FALSE;
		}

		public function test()
		{
			$res = $this->__query('SELECT COUNT(*) FROM games;');
			var_dump($res);
			self::__hardcoreDBLog();
		}
	}

	$app = new Application();
	$app->__route();
?>