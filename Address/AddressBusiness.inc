<?php

namespace Address;

/* 
 * Számlázási cím kezelő bővítés
 */

class AddressBusiness extends Address {

    //method override
    public function display() {
        $output = '<div class ="alert alert-warning business">';
        $output .= parent::display();//szülőosztály metódus hívás
        $output .= '</div>';
        return $output;
    }
    /**
     * inicializálás a címtipus beállítására
     */
    protected function _init(){
        $this->_setAddressTypeId(parent::ADDRESS_TYPE_BUSINESS);
        return;
    }
}