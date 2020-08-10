<?php
// DBダミーデータ生成ツール
// ダミーデータ生成ルールiniファイルを読んで、ランダムな値をDBのテーブルに追加する

ini_set('max_execution_time', 600);
ini_set('default_charset', 'UTF-8');
mb_internal_encoding('UTF-8');
error_reporting(E_ALL ^ E_NOTICE);
ob_end_clean();

$error_flag = false;

if (php_sapi_name() != 'cli') {
	header('Pragma: no-cache');
	header('Cache-Control: no-cache');
	echo "<html>\n\n<head>\n<title>Database insert dummy data</title>\n</head>\n\n<body>\n";
}

// ini解析
$ini = parse_ini_file('dbinsdd.ini', true);
if (!$ini) print_str("\nError: ini parse error", true);

// DB接続
$database = &$ini['*database'];
try {
	$dsn = array();
	foreach ($database as $key => &$value) {
		if (mb_substr($key, 0, 1) != '*') $dsn[] = $key . '=' . $value;
	}
	$dbh = new PDO($database['*type'] . ':' . implode(';', $dsn), $database['*username'], $database['*password']);
} catch (PDOException $e) {
	print_str("\nError: DB connection failed: " . $e->getMessage(), true);
}

// iniからDBのテーブルの項目のフォーマットを解析する
/* 解析結果は以下のように格納される
$string_format => array(
	[テーブル名] => array(
		[*count] => ダミーデータを追加する回数
		[項目名] => array(
			「|」で区切ってグループ分けしたもの
			[0] => array(
				ダミーデータ生成時、個々の要素の結果を連結する
				[0] => array(
					[type] => formatの種類
					[format] => 正規表現風のformat
								type=2の場合は以下のような配列となる。以下の例の場合、
								0.countと1.countの総計で「あ,ぃ,い,ぅ,う,ぇ,え,ぉ,お,か,20,21,22,23,24」の15種類の
								文字があることになる。そして、0始まりとして乱数で11だとすると21を取得することになる
								array(
									[0] => array(
										[value] => あ
										[count] => 10
									)
									[1] => array(
										[value] => 20
										[count] => 5
									)
									...
								)
					[format_count] => formatの各要素のcountの総計(type=2のみ)
					[min] => 最小繰り返し回数
					[max] => 最大繰り返し回数
				)
				...
			)
			...
		)
		...
	)
	...
);
*/
$string_format = array();
foreach ($ini as $table_name => &$table) {
	if (mb_substr($table_name, 0, 1) == '*') continue;
	if (preg_match('/\W/', $table_name)) {
		$error_flag = true;
		print_str("\nError: " . $table_name . ' is invalid name');
		break;
	}
	$string_format_table = &$string_format[$table_name];
	if ($string_format_table != null) {
		$error_flag = true;
		print_str("\nError: " . $table_name . ' is already exist');
		break;
	}
	$string_format_table['*count'] = 1;

	foreach ($table as $column_name => &$column) {
		if ($column_name == '*count') {
			if (preg_match('/\D/', $column)) {
				$error_flag = true;
				print_str("\nError: " . $table_name . '.*count error');
				break;
			}
			$string_format_table['*count'] = (int)$column;
			continue;
		}

		if (mb_substr($column_name, 0, 1) == '*') continue;
		if (preg_match('/\W/', $column_name)) {
			$error_flag = true;
			print_str("\nError: " . $table_name . '.' . $column_name . ' is invalid name');
			break;
		}
		$string_format_column = &$string_format_table[$column_name];
		if ($string_format_column != null) {
			$error_flag = true;
			print_str("\nError: " . $table_name . '.' . $column_name . ' is already exist');
			break;
		}
		$string_format_column = array();
		$index = 0;
		$string_format_column = &$string_format_column[$index];

		$pos1 = 0;
		$len = mb_strlen($column);
		while ($pos1 < $len) {
			switch (mb_substr($column, $pos1, 1)) {
			case '|':
				$index++;
				$string_format_column = &$string_format_table[$column_name][$index];

				$pos1++;
				break;
			case '[':
				$pos2 = mb_strpos($column, ']', $pos1 + 1);
				if ($pos2 === false) {
					$error_flag = true;
					print_str("\nError: " . $table_name . '.' . $column_name . ' parse error');
					break;
				}

				$format = mb_substr($column, $pos1 + 1, $pos2 - $pos1 - 1);
				$pos1 = $pos2 + 1;

				$count = parse_count();
				if (!$count) {
					$error_flag = true;
					print_str("\nError: " . $table_name . '.' . $column_name . ' count parse error');
					break;
				}

				// format解析
				$format_list = array();
				$format_len = mb_strlen($format);
				$format_pos1 = $format_pos2 = 0;
				$range = null;
				$number_count = 0;
				while ($format_pos1 < $format_len) {
					$curr_c = mb_substr($format, $format_pos1, 1);
					switch ($curr_c) {
						case '-':
							if ($format_pos1 == 0 || $range !== null) {
								$error_flag = true;
								print_str("\nError: " . $table_name . '.' . $column_name . ' format parse error');
								break;
							}
							if ($number_count > 0) {
								$range = (int)mb_substr($format, $format_pos2, $number_count);
								$number_count = 0;
							} else {
								$range = mb_substr($format, $format_pos1 - 1, 1);
							}
							break;
						case ',':
							if ($range !== null && $number_count == 0) {
								$error_flag = true;
								print_str("\nError: " . $table_name . '.' . $column_name . ' format parse error');
								break;
							}

							if (!parse_format_number_range()) {
								$error_flag = true;
								print_str("\nError: " . $table_name . '.' . $column_name . ' format parse error');
								break;
							}

							$range = null;
							$number_count = -1;
							break;
						case '0': case '1': case '2': case '3': case '4':
						case '5': case '6': case '7': case '8': case '9':
							if ($number_count < 0) $number_count = 0;
							if ($number_count == 0) $format_pos2 = $format_pos1;
							$number_count++;
							break;
						default:
							if ($range === null) {
								if ($format_pos1 + 1 >= $format_len || mb_substr($format, $format_pos1 + 1, 1) != '-') {
									$format_list[] = array(
										'value' => $curr_c
										, 'count' => 1
									);
								}
							} else {
								if (!is_string($range)) {
									$error_flag = true;
									print_str("\nError: " . $table_name . '.' . $column_name . ' format parse error');
									break;
								}
								$format_list[] = array(
									'value' => mb_ord($curr_c) < mb_ord($range) ? $curr_c : $range
									, 'count' => abs(mb_ord($curr_c) - mb_ord($range)) + 1
								);
							}
							$range = null;
							$number_count = 0;
							break;
					}

					if ($error_flag) break;
					$format_pos1++;
				}
				if (!parse_format_number_range()) {
					$error_flag = true;
					print_str("\nError: " . $table_name . '.' . $column_name . ' format parse error');
					break;
				}

				$format_count = 0;
				foreach ($format_list as $list) $format_count += $list['count'];
				$string_format_column[] = array(
					'type' => 2	// formatは[]で囲まていた文字列([1-10,999]、[あ-ん]等の範囲指定、[あいうえお]の中の任意の1文字)
					, 'format' => $format_list
					, 'format_count' => $format_count
					, 'min' => $count[0]
					, 'max' => $count[1]
				);
				break;
			case '{':
				$pos2 = mb_strpos($column, '}', $pos1 + 1);
				if ($pos2 === false) {
					$error_flag = true;
					print_str("\nError: " . $table_name . '.' . $column_name . ' parse error');
					break;
				}

				$format = mb_substr($column, $pos1 + 1, $pos2 - $pos1 - 1);
				$pos1 = $pos2 + 1;

				$string_format_column[] = array(
					'type' => 3	// formatは{}で囲まていた文字列(内容に応じて専用の解釈を行う)
					, 'format' => $format
					, 'min' => 1
					, 'max' => 1
				);
				break;
			default:
				$pos2 = $pos1;
				while ($pos2 + 1 < $len) {
					$c = mb_substr($column, $pos2 + 1, 1);
					if ($c == '|' || $c == '[' || $c == '{') break;
					$pos2++;
				}

				$format = mb_substr($column, $pos1, $pos2 - $pos1 + 1);
				$pos1 = $pos2 + 1;

				$count = parse_count();
				if (!$count) {
					$error_flag = true;
					print_str("\nError: " . $table_name . '.' . $column_name . ' count parse error');
					break;
				}

				$string_format_column[] = array(
					'type' => 1	// 1: formatは通常の文字列
					, 'format' => $format
					, 'min' => $count[0]
					, 'max' => $count[1]
				);
				break;
			}

			if ($error_flag) break;
		}

		if ($error_flag) break;
	}

	if ($error_flag) break;
}

if ($error_flag) exit;

$total_time = get_micro_time();
if (!$dbh->beginTransaction()) print_str("\nError: DB begin failed", true);

// ダミーデータ生成とSQL実行
foreach ($string_format as $table_name => &$table) {
	$add_count = $table['*count'];

	echo $table_name . ' ' . $add_count . ':';
	if (php_sapi_name() == 'cli') {
		echo '0';
	} else {
		echo '<span id="' . $table_name . "\">0</span>\n";
		echo '<script>var elem = document.getElementById("' . $table_name . "\")</script>\n";
	}

	$loop = 0;
	$print_count = 1;
	$next_flag = false;
	$time = get_micro_time();
	$sql = 'INSERT INTO ' . $table_name . " (\n";
	$column_count = 0;

	for (; $loop < $add_count; $loop++) {
		if ($loop % 100 == 0) print_prog($loop, $print_count, $add_count, $time, false);

		$dummy_data = array();

		foreach ($table as $column_name => &$column) {
			if (mb_substr($column_name, 0, 1) == '*') continue;

			if (!$next_flag) {
				if ($column_count > 0) $sql .= ', ';
				$sql .= $column_name . "\n";
				$column_count++;
			}
			$format_list = &$column[mt_rand(0, count($column) - 1)];
			$dummy_value = &$dummy_data[$column_name];

			foreach ($format_list as $format) {
				$count = mt_rand($format['min'], $format['max']);
				switch ($format['type']) {
				case 2:
					for (; $count > 0; $count--) {
						$idx = mt_rand(0, $format['format_count'] - 1);
						$format_count = 0;
						foreach ($format['format'] as $list) {
							if ($idx >= $format_count + $list['count']) {
								$format_count += $list['count'];
								continue;
							}

							$idx -= $format_count;
							if (is_int($list['value'])) {
								$dummy_value .= $list['value'] + $idx;
							} else {
								$dummy_value .= mb_chr(mb_ord($list['value']) + $idx);
							}
							break;
						}
					}
					break;
				case 3:
					$patt = '(\d{4}[-\/]\d{1,2}[-\/]\d{1,2})( \d{1,2}:\d{1,2}:\d{1,2})?';
					if (preg_match('/^' . $patt . '$/', $format['format'])) {
						// 日付
						$dummy_value .= $format['format'];
					} else if (preg_match('/^' . $patt . '\s*-\s*' . $patt . '$/', $format['format'], $match)) {
						// 日付範囲

						$min = strtotime($match[1] . $match[2]);
						if (!$min || $min == -1) break;
						$max = strtotime($match[3] . $match[4]);
						if (!$max || $max == -1) break;
						if ($min > $max) {
							$tmp = $min;
							$min = $max;
							$max = $tmp;
						}

						if ($match[2] != '' || $match[4] != '') {
							// 秒単位の乱数
							$dummy_value .= date('Y-m-d H:i:s', $min + mt_rand(0, $max - $min));
						} else {
							// 日単位の乱数
							$dummy_value .= date('Y-m-d', $min + round(mt_rand(0, $max - $min) / 86400) * 86400);
						}
					} else if ($format['format'] == '*newid') {
						// 追加用のid(max+1)

						$stmt_sub = $dbh->prepare('SELECT MAX(' . $column_name . ') FROM ' . $table_name);
						if (!$stmt_sub) break;
						if (!$stmt_sub->execute()) break;
						$id = $stmt_sub->fetchAll(PDO::FETCH_COLUMN, 0);
						$dummy_value .= $id[0] ? (int)$id[0] + 1 : 1;
					} else {
						// 項目名

						$dummy_value .= $dummy_data[$format['format']];
					}
					break;
				default:
					for (; $count > 0; $count--) $dummy_value .= $format['format'];
					break;
				}
			}
		}

		if (!$next_flag) {
			$sql .= ") VALUES (\n";
			for ($i = 0; $i < $column_count; $i++) {
				if ($i > 0) {
					if ($i % 10 == 0) $sql .= "\n";
					$sql .= ', ';
				}
				$sql .= '?';
			}
			$sql .= ')';

			$stmt = $dbh->prepare($sql);
			if (!$stmt) {
				$error_flag = true;
				$error = $dbh->errorInfo();
				print_str("\nError: DB prepare failed: " . $error[2]);
				break;
			}
			$next_flag = true;
		}

		$prm = array();
		foreach ($table as $column_name => &$column) {
			if (mb_substr($column_name, 0, 1) == '*') continue;
			$prm[] = $database['*charset'] == '' ? $dummy_data[$column_name] : mb_convert_encoding($dummy_data[$column_name], $database['*charset']);
		}

		if (!$stmt->execute($prm) && !$database['*ignore_add_error']) {
			$error_flag = true;
			$error = $stmt->errorInfo();
			print_str("\nError: DB execute failed: " . $error[2]);
			break;
		}
	}

	print_prog($loop, $print_count, $add_count, $time, !$error_flag);
	if ($error_flag) break;
	print_str("\n");
}

if ($error_flag) {
	$dbh->rollBack();
} else {
	if ($dbh->commit()) {
		print_str("\nCompleted (" . (get_micro_time() - $total_time) . ' sec)');
	} else {
		$error = $dbh->errorInfo();
		$dbh->rollBack();
		print_str("\nError: DB commit failed: " . $error[2]);
	}
}
echo php_sapi_name() == 'cli' ? "\n" : "\n</body>\n</html>";
exit;


// フォーマットの繰り返し回数の部分を解析する
// 返値：繰り返し回数。失敗時はfalse
function parse_count() {
	global $column, $pos1;

	if (mb_substr($column, $pos1, 1) != '{') return array(1, 1);

	$pos2 = mb_strpos($column, '}', $pos1 + 1);
	if ($pos2 === false) return false;
	$count = mb_substr($column, $pos1 + 1, $pos2 - $pos1 - 1);

	if (preg_match('/^(\d+)$/', $count, $match)) {
		$pos1 = $pos2 + 1;
		return array((int)$match[1], (int)$match[1]);
	} else if (preg_match('/^(\d+),(\d+)$/', $count, $match) && (int)$match[1] <= (int)$match[2]) {
		$pos1 = $pos2 + 1;
		return array((int)$match[1], (int)$match[2]);
	}

	return false;
}

// type=2のformatの数字範囲の部分を解析する
// 返値：成功ならtrue、それ以外はfalse
function parse_format_number_range() {
	global $format_list, $format, $number_count, $range, $format_pos2;

	if ($number_count > 0) {
		$value = (int)mb_substr($format, $format_pos2, $number_count);

		if ($range === null) {
			$format_list[] = array(
				'value' => $value
				, 'count' => 1
			);
		} else {
			if (!is_int($range)) return false;

			$format_list[] = array(
				'value' => $value < $range ? $value : $range
				, 'count' => abs($value - $range) + 1
			);
		}
	} else if ($number_count < 0) {
		$format_list[] = array(
			'value' => ''
			, 'count' => 1
		);
	}

	return true;
}

// マルチバイト対応ord
// 引数：c 文字
// 返値：数値
function mb_ord($c) {
	$n = 0;
	$len = strlen($c);
	for ($i = 0; $i < $len; $i++) $n += ord($c[$i]) * pow(256, $len - $i - 1);
	return $n;
}

// マルチバイト対応chr
// 引数：n 数値
// 返値：文字
function mb_chr($n) {
	$c = '';
	if ($n > 0xffffff) $c .= chr($n >> 24 & 0xff);
	if ($n > 0xffff) $c .= chr($n >> 16 & 0xff);
	if ($n > 0xff) $c .= chr($n >> 8 & 0xff);
	$c .= chr($n & 0xff);
	return $c;
}

// PHP-SAPIを考慮したecho
// 引数：str 出力する文字列
//       exit_flag trueにすると出力後処理終了する
function print_str($str, $exit_flag = false) {
	$str = mb_convert_encoding($str, mb_internal_encoding(), 'JIS,UTF-8,eucJP-win,SJIS-win');
	if (php_sapi_name() == 'cli') {
		echo $str;
		if ($exit_flag) {
			echo "\n";
			exit;
		}
	} else {
		echo str_replace("\n", "<br>\n", $str);
		if ($exit_flag) {
			echo "\n</body>\n</html>";
			exit;
		}
	}
}

// 進捗を出力
// 引数：count 出力したい数値
//       print_count 出力済みの文字数
//       total 件数
//       time 時間
//       comp_flag 完了ならtrueを指定する
function print_prog($count, &$print_count, $total, $time, $comp_flag) {
	$time = get_micro_time() - $time;
	if ($comp_flag) {
		$count = $count . ' (' . $time . ' sec)';
	} else {
		if ($count > 0) $count = $count . ' (remainder ' . round($total / $count * $time - $time) . ' sec)';
	}

	if (php_sapi_name() == 'cli') {
		echo str_repeat(chr(8), $print_count) . $count;
		$count = strlen($count);
		$diff = $print_count - $count;
		if ($diff > 0) echo str_repeat(' ', $diff) . str_repeat(chr(8), $diff);
		$print_count = $count;
	} else {
		echo '<script>elem.innerHTML = "' . $count . "\";</script>\n";
		flush();
	}
}

// 現在の時間をマイクロ秒で取得
// 返値：マイクロ秒
function get_micro_time() {
	list($usec, $sec) = explode(' ', microtime());
	return (float)$usec + (float)$sec;
}
