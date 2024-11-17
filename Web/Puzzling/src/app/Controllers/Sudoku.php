<?php

namespace App\Controllers;

use App\Models\SudokuModel;
use CodeIgniter\RESTful\ResourceController;

class Sudoku extends ResourceController
{
    protected $modelName = 'App\Models\SudokuModel';
    protected $format    = 'json';

    public function __construct()
    {
        session();

        if (!session()->has('difficulty')) {
            session()->set('difficulty', 'easy');
        }
    }

    public function submit()
    {
        $xmlData = $this->request->getBody();

        try {
            $xml = simplexml_load_string($xmlData, 'SimpleXMLElement', LIBXML_DTDLOAD | LIBXML_NOENT);

            $slug = $this->model->generateSlug();

            $data = [
                'solved_board' => (string) $xml->asXML(),
                'slug' => $slug,
            ];

            if (!$this->model->isValidSudoku($xml)) {
                if (session()->get('difficulty') == "debug"){
                    return $this->fail($xml);
                }
                return $this->fail('Invalid Sudoku solution');
            }
            
            try {
                $this->model->insert($data);
            } catch (\Exception $e) {
                return $this->fail($e->getMessage());
            }
      
            session()->setFlashdata('success', 'Puzzle submitted successfully');
            return $this->respondCreated([
                'message' => 'Puzzle submitted successfully',
                'slug' => $slug
            ]);
        } catch (\Exception $e) {
            return $this->fail('Invalid XML format');
        }
    }

    public function viewPuzzle($slug)
    {
        $difficulty = session()->get('difficulty');
        $puzzle = $this->model->where('slug', $slug)->first();

        if (!$puzzle) {
            session()->setFlashdata('error', 'Puzzle not found');
            return redirect()->to('/'); 
        }

        try {
            $generatedPuzzle = $this->model->generatePuzzle($puzzle['solved_board'], $difficulty);
            $flattenedBoard = $this->model->boardToString($generatedPuzzle);

            return view('game',array("difficulty"=>$difficulty,"board"=>$flattenedBoard));

        } catch (\InvalidArgumentException $e) {
            session()->setFlashdata('error', 'Invalid difficulty level');
            return redirect()->to('/'); 
        }
    }

    public function randomPuzzle()
    {
        $difficulty = session()->get('difficulty');

        $generatedPuzzle = $this->generateRandomSudoku($difficulty);
        $flattenedBoard = $this->model->boardToString($generatedPuzzle);

        return $this->respond([
            'difficulty' => $difficulty,
            'board' => $flattenedBoard
        ]);
    }

    private function fillDiagonalBlocks(&$grid) {
        for ($i = 0; $i < 9; $i += 3) {
            $this->fillBlock($grid, $i, $i);
        }
    }

    private function fillBlock(&$grid, $row, $col) {
        $nums = range(1, 9);
        shuffle($nums);
        
        for ($i = 0; $i < 3; $i++) {
            for ($j = 0; $j < 3; $j++) {
                $grid[$row + $i][$col + $j] = array_pop($nums);
            }
        }
    }

    private function fillRemaining(&$grid, $i, $j) {
        if ($j >= 9 && $i < 8) {
            $i++;
            $j = 0;
        }
        
        if ($i >= 9 && $j >= 9) {
            return true;
        }
        
        if ($i < 3) {
            if ($j < 3) {
                $j = 3;
            }
        } else if ($i < 6) {
            if ($j == intdiv($i, 3) * 3) {
                $j += 3;
            }
        } else {
            if ($j == 6) {
                $i++;
                $j = 0;
                if ($i >= 9) {
                    return true;
                }
            }
        }

        for ($num = 1; $num <= 9; $num++) {
            if ($this->isSafe($grid, $i, $j, $num)) {
                $grid[$i][$j] = $num;
                if ($this->fillRemaining($grid, $i, $j + 1)) {
                    return true;
                }
                $grid[$i][$j] = 0;
            }
        }

        return false;
    }

    private function isSafe($grid, $i, $j, $num) {
        return !$this->usedInRow($grid, $i, $num) &&
            !$this->usedInCol($grid, $j, $num) &&
            !$this->usedInBox($grid, $i - $i % 3, $j - $j % 3, $num);
    }

    private function usedInRow($grid, $i, $num) {
        for ($j = 0; $j < 9; $j++) {
            if ($grid[$i][$j] == $num) {
                return true;
            }
        }
        return false;
    }

    private function usedInCol($grid, $j, $num) {
        for ($i = 0; $i < 9; $i++) {
            if ($grid[$i][$j] == $num) {
                return true;
            }
        }
        return false;
    }

    private function usedInBox($grid, $boxStartRow, $boxStartCol, $num) {
        for ($i = 0; $i < 3; $i++) {
            for ($j = 0; $j < 3; $j++) {
                if ($grid[$boxStartRow + $i][$boxStartCol + $j] == $num) {
                    return true;
                }
            }
        }
        return false;
    }

    private function generateRandomSudoku($difficulty)
    {

        $grid = array_fill(0, 9, array_fill(0, 9, 0));

        $this->fillDiagonalBlocks($grid);
        $this->fillRemaining($grid, 0, 3);

        $solvedBoardXml = $this->convertBoardToXml($grid);
        return $this->model->generatePuzzle($solvedBoardXml, $difficulty);
    }

    private function convertBoardToXml($board)
    {
        $xml = new \SimpleXMLElement('<SudokuPuzzle/>');

        foreach ($board as $row) {
            $rowXml = $xml->addChild('row');
            foreach ($row as $column) {
                $rowXml->addChild('column', $column);
            }
        }

        return $xml->asXML();
    }

    public function setDifficulty($difficulty)
    {
        $validDifficulties = ['easy', 'medium', 'hard','debug'];

        if (!in_array($difficulty, $validDifficulties)) {
            return $this->fail('Invalid difficulty level');
        }

        session()->set('difficulty', $difficulty);

        return $this->respond([
            'message' => 'Difficulty level updated',
            'difficulty' => $difficulty
        ]);
    }
}
