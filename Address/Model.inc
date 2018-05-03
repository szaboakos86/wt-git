<?php
namespace Address;
use Database\Database as Database;

/**
 * Application CRUD interface
 */
interface Model {
    //Model betöltése
    public static function load($id);
    
    //Minden elem lekérée
    public static function all();

    //Model mentése
    public function save();
    
    //Model törlése
    public function delete();
    
    //Model frissítése
    public function update();
    
}