<?php

use CodeIgniter\Router\RouteCollection;

/**
 * @var RouteCollection $routes
 */
$routes->get('/', 'Home::index');
$routes->get('sudoku/setDifficulty/(:segment)', 'Sudoku::setDifficulty/$1');
$routes->post('sudoku/submit', 'Sudoku::submit');
$routes->get('sudoku/puzzles', 'Sudoku::listPuzzles');
$routes->get('sudoku/puzzle/(:segment)', 'Sudoku::viewPuzzle/$1');
$routes->get('sudoku/random', 'Sudoku::randomPuzzle');



