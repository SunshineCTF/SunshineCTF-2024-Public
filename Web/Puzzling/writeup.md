# Puzzling

### Writeup

This is an exercise in source code analysis and understanding XXE attacks in PHP. Players are shown a simple Sudoku puzzle site which allows them to play random puzzles as well as submit puzzles to share with others.

Analyzing the source code, which is running the Codeigniter 4 MVC framework, they will find that the submission function for sending in new puzzles is vulnerable to XML external entity injection. By setting the difficulty to "debug" by browsing to the correct URL, they can see the debug output for the submission function in the event that their submission is not a valid solvable Sudoku solution.

In the Sudoku controller (app/Controllers/Sudoku.php), there is a function for setting the difficulty of the puzzle, which is called via Javascript using the pulldown menu on the game page.

```php
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
```

So, by browsing to */sudoku/setDifficulty/debug*, the player can set the debug mode which also makes the puzzles trivial to solve.

Next, they may notice that the submittion function in the same controller has a case where if the provided board is not a valid Sudoku puzzle, it will return an error message. However, if the debug option is set, it will print the XML output instead.

```php
if (!$this->model->isValidSudoku($xml)) {
    if (session()->get('difficulty') == "debug"){
        return $this->fail($xml);
    }
    return $this->fail('Invalid Sudoku solution');
}
```

Using this, they can inject an entity which loads the content of /flag.txt into the XML, which is then printed in the debug output. This can be automated with [solution/solve.py](solve.py). Manually, this can be done by modifying the contents of the provided sample board XML to add the malicious external entity:


```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
  <!ENTITY file SYSTEM "file:///flag.txt">
]>
<SudokuPuzzle>
    <row>
        <column>&file;</column>
```

Since the debug output happens after any entities are resolved, the contents of flag.txt are printed in the cell instead of a number.
