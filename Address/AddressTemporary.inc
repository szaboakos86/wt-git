<?php

namespace Address;

/* 
 * Ideiglenes cím kezelő bővítés
 */

class AddressTemporary extends Address {

    //method override
    public function display() {
        $output = '<div class ="alert alert-success business">';
        $output .= parent::display();//szülőosztály metódus hívás
        $output .= '</div>';
        return $output;
    }
    /**
     * inicializálás a címtipus beállítására
     */
    protected function _init(){
        $this->_setAddressTypeId(parent::ADDRESS_TYPE_TEMPORARY);
        return;
    }
}