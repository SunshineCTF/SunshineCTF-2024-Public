<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sudoku Game</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Comic+Neue:ital,wght@0,300;0,400;0,700;1,300;1,400;1,700&display=swap" rel="stylesheet">
    <style>
        body,html {
            background-color: #87CEEB;
            font-family: "Comic Neue", cursive;
        }
        #header {
            font-weight:700;
            color:#FFF;
            text-shadow:0px 0px 3px yellow;
        }
        .sudoku-board {
            display: grid;
            grid-template-columns: repeat(9, 1fr);
            gap: 5px;
            max-width: 360px;
            margin: auto;
        }
        .sudoku-cell {
            width: 40px;
            height: 40px;
            text-align: center;
            font-size: 20px;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        .sudoku-cell:focus {
            outline: none;
            border-color: #007bff;
        }
        .banner {
            text-align: center;
            margin-top: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1 class="text-center my-4" id="header">Sunshine Sudoku</h1>

        <div class="text-center mb-4">
            <select id="difficultySelector" class="form-select w-25 mx-auto">
                <option value="easy"<?=($difficulty == "easy" ? " selected" : "")?>>Easy</option>
                <option value="medium"<?=($difficulty == "medium" ? " selected" : "")?>>Medium</option>
                <option value="hard"<?=($difficulty == "hard" ? " selected" : "")?>>Hard</option>
            </select>
        </div>

        <div id="sudokuBoard" class="sudoku-board"></div>
        <div class="text-center my-4">
            <button id="newGameButton" class="btn btn-primary">New Game</button>
        </div>
        <div class="text-center my-4">
            <p><em>Got a fun puzzle? Submit it to share with others. Click <a href="/example.xml">here</a> to download an example board file.</em></p>
            <input type="file" id="xmlFileInput" accept=".xml" class="form-control-file mb-3">
            <button id="submitGameButton" class="btn btn-primary">Submit Board</button>
        </div>

        <div id="winBanner" class="banner"></div>
        <div class="alert alert-danger"<?=(session()->getFlashdata("error") ? "" : "style='display:none'")?>><?=session()->getFlashdata("error")?></div>
        <div class="alert alert-success"<?=(session()->getFlashdata("success") ? "" : "style='display:none'")?>><?=session()->getFlashdata("success")?></div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/axios/dist/axios.min.js"></script>
    <script>
        const boardElement = document.getElementById('sudokuBoard');
        const winBanner = document.getElementById('winBanner');
        const difficultySelector = document.getElementById('difficultySelector');
        const newGameButton = document.getElementById('newGameButton');
        var gameOver = false;
        <?php
        
        if(isset($board)){
        ?>
        const initBoard = "<?=$board?>";
        const initDifficulty = "<?=$difficulty?>";
        <?
        } else {
        ?>
        const initBoard = false;
        const initDifficulty = "easy";
        <?
        }
        
        ?>;

        function createSudokuBoard(board) {
            boardElement.innerHTML = '';
            board.split('').forEach((char, index) => {
                const input = document.createElement('input');
                input.type = 'text';
                input.maxLength = 1;
                input.className = 'sudoku-cell';
                input.value = char === '.' ? '' : char;
                input.disabled = char !== '.';

                input.addEventListener('input', () => {
                    if (isBoardFilled()) {
                        checkWin();
                    }
                });

                boardElement.appendChild(input);
            });
            gameOver = false;
        }

        function isBoardFilled() {
            const cells = Array.from(boardElement.children);
            return cells.every(cell => /^[1-9]$/.test(cell.value));
        }

        function getBoardState() {
            return Array.from(boardElement.children).map(cell => cell.value || '.');
        }

        function checkWin() {
            if (isBoardFilled()) {
                const boardArray = getBoardState();
                if (isValidSudoku(boardArray)) {
                    winBanner.innerHTML = '<div class="alert alert-success">Congratulations! You have solved the puzzle!</div>';
                    gameOver = true;
                }
            }
        }

        function isValidSudoku(boardArray) {
            const rows = Array(9).fill(null).map(() => []);
            const cols = Array(9).fill(null).map(() => []);
            const boxes = Array(9).fill(null).map(() => []);

            for (let i = 0; i < 81; i++) {
                const value = boardArray[i];
                if (value === '.') continue;

                const row = Math.floor(i / 9);
                const col = i % 9;
                const box = Math.floor(row / 3) * 3 + Math.floor(col / 3);

                if (rows[row].includes(value) || cols[col].includes(value) || boxes[box].includes(value)) {
                    return false;
                }

                rows[row].push(value);
                cols[col].push(value);
                boxes[box].push(value);
            }

            return true; 
        }

        function fetchSudokuBoard() {
            
            axios.get('/sudoku/random')
                .then(response => {
                    createSudokuBoard(response.data.board);
                    winBanner.innerHTML = '';
                })
                .catch(error => console.error('Error fetching Sudoku board:', error));
        }

        difficultySelector.addEventListener('change', () => {

            if(!confirm("Are you sure you want to start a new game?")){
                return;
            }

            const selectedDifficulty = difficultySelector.value;
            axios.get(`/sudoku/setDifficulty/${selectedDifficulty}`)
                .then(() => fetchSudokuBoard())
                .catch(error => console.error('Error setting difficulty:', error));
        });

        newGameButton.addEventListener('click',() => {

            if(!confirm("Are you sure you want to start a new game?")){
                return;
            }

            fetchSudokuBoard();
        });

        document.getElementById('submitGameButton').addEventListener('click', () => {
            const fileInput = document.getElementById('xmlFileInput');
            const file = fileInput.files[0];

            if (!file) {
                alert("Please select an XML file to submit.");
                return;
            }

            const reader = new FileReader();
            reader.onload = function(event) {
                const xmlContent = event.target.result;

                axios.post('/sudoku/submit', xmlContent, {
                    headers: {
                        'Content-Type': 'application/xml'
                    }
                })
                .then(response => {
                    const slug = response.data.slug;
                    if (slug) {
                        window.location.href = `/sudoku/puzzle/${slug}`;
                    } else {
                        alert("Submission was successful, but no slug was returned.");
                    }
                })
                .catch(error => {
                    console.error('Error submitting Sudoku game:', error);
                    alert("There was an error submitting the game. Please try again.");
                });
            };

            reader.readAsText(file);
        });


        if(initBoard){
            createSudokuBoard(initBoard);
        } else {
            fetchSudokuBoard();
        }

    </script>
</body>
</html>
