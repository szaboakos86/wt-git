<?php

namespace Address;
use Database\Database as Database;
//use Address\AddressBusiness;
//use Address\AddressResidence;
//use Address\AddressTemporary;
use Address\Model;
//use Address\AddressBusiness as AddressBusiness;
//use Address\AddressResidence as AddressResidence;
//use Address\AddressTemporary as AddressTemporary;

/**
 * Címkezelő osztály
 */

abstract class Address implements Model {
    //konstansok a címtipusok azonosítására
    const ADDRESS_TYPE_RESIDENCE = 1;
    const ADDRESS_TYPE_BUSINESS = 2;
    const ADDRESS_TYPE_TEMPORARY = 3;
    
    //hibakódok
    const ADDRESS_ERROR_NOT_FOUND = 1000;
    const ADDRESS_ERROR_INVALID_TYPE_ID = 1001;
    
    
    //statikus tulajdonságú tömb a címtipusok tárolására, ez példányosítás nélkül osztályból elérhető
    public static $valid_address_types = [
        self::ADDRESS_TYPE_RESIDENCE => 'Residence',
        Address::ADDRESS_TYPE_BUSINESS => 'Business',
        self::ADDRESS_TYPE_TEMPORARY => 'Temporary',
    ];
    //címsor 1
    public $address_line_1;
    //címsor 2
    public $address_line_2;
    //irsz
    protected $_postal_code;
    //ország
    public $country_name;
    //város
    public $city_name;
    //városrész
    public $subdivision_name;
    
    //cím azonosító
    protected $_address_id;
    //cím tipus azonosito
    protected $_address_type_id;

    //timestamp
    protected $_time_created = 12345;
    protected $_time_updated = 67890;
    
    /**
     * Konstruktor, akkor fut, amikor pélány készül (new)
     */
    public function __construct($data=[]) {
        //die('fut a constructor');
        $this->_time_created = time();
        $this->_init();//inicializálás híváa
        if(is_array($data)){//ha kaptunk adatokat és az ömb, megpróbáljuk felépíteni az objektumot, ha nem jó vmi, trigger error
            foreach ($data as $property => $value){
                if(in_array($property, [
                    'address_id', 'time_created', 'time_updated'
                ])){//kivételek a db ből betöltött védett nevekhez
                    $property = '_' . $property;
                }
                $this->$property = $value;
            }
        } else {
            trigger_error("A kapott adatokból nem építhető fel az objektum");
        }
    }
    
    /**
     * getter- akkor fut, amikor nem létező vagy védett tulajdonságot próbálunk kiolvasni
     * @param string $name
     * @return mixed
     */
    public function __get($name) {
        // ha nem lenne irsz, keressünk...
        if(!$this->_postal_code){
            $this->_postal_code = $this->_postal_code_search();
        }
       
        $protected_property_name = "_" . $name;  //ha van védett tulajdonság, akkor az elnevezési konvenció miatt így lenne nevezve
        
        if(property_exists($this, $protected_property_name)){
            return $this->$protected_property_name; //!!!!!!!!!!!!!!!!!!
        }
        
        trigger_error("Nem létező védett tulajdonságot próbálsz elérni (__get): $name");
        //die("Fut a get: $name");
    }
    
    /**
     * setter - akkor fut amikor nem létező vagy védett tulajdonságot próbálunk beállítani
     * @param string $name
     * @param mixed $value
     * @return void
     */
    public function __set($name, $value) {
        //kivétel megadása hogy 'kívülről' lehessen felvenni eddig nem létező postal_code tulajdonságot
        if($name == 'postal_code'){
            $this->$name = $value;
            return;
        }
        //címtipus átvétele
        if($name == 'address_type_id'){
            $this->_setAddressTypeId($value);
            return;
        }
        trigger_error("Nem létező védett tulajdonságnak próbálsz értéket adni (__set): $name | $value");
        //die("Fut a set: $name | $value");
    }
    
    /**
     * Objektum echo esetén fut le, ha nincs megírva akkor catchable fatal error áll meg objektum echozásakor
     * @return string
     */
    public function __toString() {
        return $this->display();
    }
    
    abstract protected function _init(); //_init_metódus megkövetelése a bővítésből
    
    /**
     * Irányítószám keresés város és városrész alapján
     * @todo: write the database logic
     * @return string
     */
    protected function _postal_code_search(){
        $db = Database::getInstance(); //példányosítás
        
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        $qry = "SELECT irsz"
                . " FROM telepulesek";
        //kapott adatok escapelése
        $city_name = $mysqli->real_escape_string($this->city_name);
        $subdivision_name = $mysqli->real_escape_string($this->subdivision_name);
        $qry .= " WHERE telepules_nev = '$city_name' "
            . "AND telepules_resz = '$subdivision_name'";
        //$qry = "SELECT irsz FROM telepulesek WHERE telepules_nev = 'Füzesabony' AND telepules_resz = '';";
        $result = $mysqli->query($qry); //lekérés
        $row = $result->fetch_row(); //kibontás
        if($row) {
            return $row[0];
        }

        return 'nem találtam';
    }
    
    public static function find_postal_code($input_city_name, $input_subdivision_name){
        $db = Database::getInstance(); //példányosítás
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        //kapott adatok escapelése
        $city_name = $mysqli->real_escape_string($input_city_name);
        $subdivision_name = $mysqli->real_escape_string($input_subdivision_name);
        $qry = "SELECT irsz"
                . " FROM telepulesek "
                . "WHERE telepules_nev = '$city_name' "
                . "AND telepules_resz = '$subdivision_name'";
        $result = $mysqli->query($qry); //lekérés
        $row = $result->fetch_row(); //kibontás
        if($row) {
            return $row[0];
        }

        return '';
    }
    
    public static function find_postal_code_new($input_city_name, $input_subdivision_name){
        $db = Database::getInstance(); //példányosítás
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        //kapott adatok escapelése
        $city_name = $mysqli->real_escape_string($input_city_name);
        $subdivision_name = $mysqli->real_escape_string($input_subdivision_name);
        $qry = "SELECT irsz, telepules_nev, telepules_resz"
                . " FROM telepulesek "
                . "WHERE telepules_nev = '$city_name' "
                . "AND telepules_resz = '$subdivision_name'";
        $result = $mysqli->query($qry); //lekérés
        $row = $result->fetch_assoc(); //kibontás
        if($row) {
            return $row;
        }

        return 'Nem található bejegyzés';
    }
    
    public static function find_city($input_city_name){
        $db = Database::getInstance(); //példányosítás
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        $city_name = $mysqli->real_escape_string($input_city_name);
        $qry = "SELECT telepules_nev"
                . " FROM telepulesek"
                . " WHERE telepules_nev LIKE '$city_name%'";
        $list = '';
        if ($result = $mysqli->query($qry)) {
            //$list .= '<div class="list-group">';
            //while ($row = $result->fetch_row()) {
            //    $list .= '<a href="#" class="list-group-item">' . $row[0] . '</a>';
            //}
            //$list .= '</div>';
			$list .= '<ul id="varoslistazas">';
            while ($row = $result->fetch_row()) {
                $list .= '<li class="list-city" data-value="' . $row[0] . '">' . $row[0] . '</li>';
            }
            $list .= '</ul>';
        }
        return $list;
    }
    
    /**
     * Címpéldány 'kiírás'
     * @return string
     */
    public function display(){
        //Címsor 1
        $output = $this->address_line_1;
        //ha van címsor 2
        if($this->address_line_2) {
            $output .= '<br>'.$this->address_line_2;
        }
        //város, városrész ha van
        $output .= '<br>' . $this->city_name;
        $output .= $this->subdivision_name ? ", $this->subdivision_name" : "";
        //város
        $output .= '<br>' . $this->country_name;
        //irsz
        $output .= '<br>' . $this->postal_code;
        
        return $output;         
    }
    
    /**
     * Címtipus azonosító ellenőrzése - akár kívülről is, példány nélkül hívható
     * @param int $address_type_id
     * @return bool
     */
    public static function isValidAddressTypeId(int $address_type_id) {
        return array_key_exists($address_type_id, self::$valid_address_types);
    }
    
    
    /**
     * Címtipus azonosító védett tulajdonság beállítása
     * @param int $address_type_id
     * @return void
     */
    protected function _setAddressTypeId($address_type_id) {
        if(self::isValidAddressTypeId($address_type_id)) {
            $this->_address_type_id = $address_type_id;
            
        } else {
            trigger_error("A kapott azonosító nem érvényes.");
        }
        return;
        
    }
    
    //Address példányosítása
    public static function getInstance($address_type_id, $data) {
        // Osztály metódus felhasználása esetleges adatsérülés miatt
        if(!self::isValidAddressTypeId($address_type_id)){
            throw new ExceptionAddress('Érvénytelen címtipus, nem lehet a kért elemet pédányosítani (' . $data['address_id'] . ' - address_type_id:' . $address_type_id . ')', self::ADDRESS_ERROR_INVALID_TYPE_ID);
        }
        $class_name = 'Address\\Address'.self::$valid_address_types[$address_type_id];
        //ha sérült address_type_id van (nem érvényes) akkor vizsgáljuk meg hogy az adott class pédánosítható-e
//        $reflector = new ReflectionClass($class_name);
//        if($reflector->isAbstract()){
//            throw new ExceptionAddress('Érvénytelen címtipus, nem lehet a kért elemet pédányosítani (' . $data['address_id'] . ' - address_type_id:' . $address_type_id . ')', self::ADDRESS_ERROR_INVALID_TYPE_ID);
//        }
        //die(var_export($class_name) . '---' . var_export($data));
        return new $class_name($data);
        die('ok');
    } 

    


    //interface által megkövetelt eljárások
    //Összes cím lekérése listázáshoz
    final public static function all(){
        
        
        return addresses;
    }
    //Cím betöltése azonosító alapján
    final public static function load($id){
        //echo 'BEt'.$id;
        $db = Database::getInstance(); //példányosítás
        
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        $qry = "SELECT* FROM addresses "
                . "WHERE address_id = " . (int) $id . " LIMIT 1";
        $result = $mysqli->query($qry); //lekérés
        if($row = $result->fetch_assoc()){
            $address = self::getInstance($row['address_type_id'], $row); // példányosítás és bővítés hívásával
            return $address;
        }
        
        throw new ExceptionAddress('Nem sikerült betölteni: - ' . $id, self::ADDRESS_ERROR_NOT_FOUND);
        return ;
    }
    
    //Cím mentése
    final public function save(){
        $db = Database::getInstance(); //példányosítás
        
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        $qry = "INSERT INTO addresses ("
                . "address_type_id,"
                . "address_line_1,"
                . "address_line_2,"
                . "country_name,"
                . "city_name,"
                . "subdivision_name,"
                . "postal_code,"
                . "time_created,"
                . "time_updated) VALUES ("
                . "'{$this->_address_type_id}',"
                . "'{$mysqli->real_escape_string($this->address_line_1)}',"
                . "'{$mysqli->real_escape_string($this->address_line_2)}',"
                . "'{$mysqli->real_escape_string($this->country_name)}',"
                . "'{$mysqli->real_escape_string($this->city_name)}',"
                . "'{$mysqli->real_escape_string($this->subdivision_name)}',"
                . "'{$mysqli->real_escape_string($this->postal_code)}',"
                //. "'{$this->_time_created}',"
                . "'" . date("Y-m-d H:i:s",$this->_time_created) . "',"
                . "'{$this->_time_updated}')";
    $result = $mysqli->query($qry) or die('SNAFU' . $mysqli->error());
    if ($mysqli->insert_id) {
        $this->_address_id = $mysqli->insert_id;
    }
    
    }
    
    //Cím törlése
    final public function delete(){
        $db = Database::getInstance(); //példányosítás
        
        $mysqli = $db->getConnection();
        $qry = "DELETE FROM addresses WHERE address_id = {$this->_address_id} LIMIT 1";
        $mysqli->query($qry);
        
        return;
    }
    
    //Cím frissítése
    final public function update(){
        $db = Database::getInstance(); //példányosítás
        
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        $qry = "UPDATE `addresses` SET "
                . "`address_type_id` = '{$this->_address_type_id}', "
                . "`address_line_1` = '{$mysqli->real_escape_string($this->address_line_1)}', "
                . "`address_line_2` = '{$mysqli->real_escape_string($this->address_line_2)}', "
                . "`country_name` = '{$mysqli->real_escape_string($this->country_name)}', "
                . "`city_name` = '{$mysqli->real_escape_string($this->city_name)}', "
                . "`subdivision_name` = '{$mysqli->real_escape_string($this->subdivision_name)}', "
                //. "`postal_code` = '{$mysqli->real_escape_string($this->_postal_code)}', "
                . "`postal_code` = '{$mysqli->real_escape_string(self::find_postal_code($this->city_name, $this->subdivision_name))}', "
                //. "`time_created` = '2017-11-19 11:23:01', "
                . "`time_updated` = '" . date("Y-m-d H:i:s",time()) . "' "
                . "WHERE "
                . "`address_id` = {$this->_address_id};";
        $mysqli->query($qry) or die('SNAFU' . $mysqli->error());
    }
    
    //Az összes betöltése
    final public static function listAll(){
        $db = Database::getInstance(); //példányosítás
        
        $mysqli = $db->getConnection();
        $mysqli->set_charset('utf8');//kódlap illesztése
        $qry = "SELECT* FROM addresses";
        $result = $mysqli->query($qry);
        if($list = $result->fetch_all(MYSQLI_ASSOC)){
            return $list;
        }
        
        throw new ExceptionAddress('Nem sikerült betölteni a listát', self::ADDRESS_ERROR_NOT_FOUND);
        return ;
    }

    

}
