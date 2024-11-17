<?php

namespace App\Controllers;

class Home extends BaseController
{

    public function __construct()
    {
        session();

        if (!session()->has('difficulty')) {
            session()->set('difficulty', 'easy');
        }
    }

    public function index(): string
    {
        $difficulty = session()->get('difficulty');

        return view('game',array("difficulty"=>$difficulty));
    }
}
