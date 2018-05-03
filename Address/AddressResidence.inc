<?php
namespace Address;
/* 
 * Állandó cím kezelő bővítés
 */

class AddressResidence extends Address {
    //property redeclare
    public $country_name = "Scotland";
    
    //method override
    public function display() {
        $output = '<div class ="alert alert-info residence">';
        $output .= parent::display();//szülőosztály metódus hívás
        $output .= '</div>';
        return $output;
    }
    /**
     * inicializálás a címtipus beállítására
     */
    protected function _init(){
        $this->_setAddressTypeId(parent::ADDRESS_TYPE_RESIDENCE);
        return;
    }
}