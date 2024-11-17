<?php

namespace App\Database\Migrations;

use CodeIgniter\Database\Migration;

class CreateSudokuPuzzles extends Migration
{
    public function up()
    {
        $this->forge->addField([
            'id' => [
                'type'           => 'INT',
                'unsigned'       => true,
                'auto_increment' => true,
            ],
            'solved_board' => [
                'type' => 'TEXT',
                'null' => false,
            ],
            'slug' => [
                'type' => 'VARCHAR',
                'constraint' => 255,
                'null' => false,
                'unique' => true,
            ],
            'created_at' => [
                'type' => 'TIMESTAMP',
                'default' => 'CURRENT_TIMESTAMP',
            ],
        ]);
        $this->forge->addKey('id', true);
        $this->forge->createTable('sudoku_puzzles');
    }

    public function down()
    {
        $this->forge->dropTable('sudoku_puzzles');
    }
}
