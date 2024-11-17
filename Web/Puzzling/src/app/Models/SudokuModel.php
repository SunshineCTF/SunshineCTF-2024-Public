<?php

namespace App\Models;

use CodeIgniter\Model;

class SudokuModel extends Model
{
    protected $table = 'sudoku_puzzles';
    protected $primaryKey = 'id';
    protected $allowedFields = ['solved_board', 'slug'];

    public function generateSlug($length = 8)
    {
        return bin2hex(random_bytes($length / 2));
    }

    public function isValidSudoku($xml)
    {
        $board = $this->parseXmlToBoard($xml);

        if (!$this->isValidBoard($board)) {
            return false;
        }

        for ($i = 0; $i < 9; $i++) {
            if (!$this->isValidSet($board[$i])) {
                return false;
            }
        }

        for ($i = 0; $i < 9; $i++) {
            $column = array_column($board, $i);
            if (!$this->isValidSet($column)) {
                return false;
            }
        }

        for ($row = 0; $row < 9; $row += 3) {
            for ($col = 0; $col < 9; $col += 3) {
                $subGrid = [];
                for ($i = 0; $i < 3; $i++) {
                    for ($j = 0; $j < 3; $j++) {
                        $subGrid[] = $board[$row + $i][$col + $j];
                    }
                }
                if (!$this->isValidSet($subGrid)) {
                    return false;
                }
            }
        }

        return true;
    }

    private function parseXmlToBoard($xml)
    {
        $board = [];

        foreach ($xml->row as $row) {
            $boardRow = [];
            foreach ($row->column as $column) {
                $boardRow[] = (int)$column;
            }
            $board[] = $boardRow;
        }

        return $board;
    }

    private function isValidSet($set)
    {
        sort($set);
        return $set === [1, 2, 3, 4, 5, 6, 7, 8, 9];
    }

    private function isValidBoard($board)
    {
        if (count($board) !== 9) {
            return false;
        }

        foreach ($board as $row) {
            if (count($row) !== 9) {
                return false;
            }
        }

        return true;
    }

    public function generatePuzzle($solvedBoard, $difficulty)
    {
        $board = $this->parseXmlToBoard(simplexml_load_string($solvedBoard));
        $totalSpaces = 81;

        $blankSpaces = [
            'easy' => 31,
            'medium' => 51,
            'hard' => 61,
            'debug' => 10
        ];

        if (!array_key_exists($difficulty, $blankSpaces)) {
            throw new \InvalidArgumentException('Invalid difficulty level');
        }

        $spacesToBlank = $blankSpaces[$difficulty];
        $blankedPositions = array_rand(range(0, $totalSpaces - 1), $spacesToBlank);

        foreach ($blankedPositions as $position) {
            $row = intdiv($position, 9);
            $col = $position % 9;
            $board[$row][$col] = 0;
        }

        return $board;
    }

    public function boardToString($board)
    {
        $flattened = '';
        foreach ($board as $row) {
            foreach ($row as $cell) {
                $flattened .= $cell === 0 ? '.' : $cell;
            }
        }
        return $flattened;
    }
}
