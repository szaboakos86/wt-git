<?php

namespace Address;
/* 
 * Saját kivételkezelések
 */

class ExceptionAddress extends \Exception {
    
    public function __toString() {
        $msg = __CLASS__ . ":" . $this->getMessage() . " - errorcode[" . $this->code . "]";
        
        return $msg;
    }
    
    
}
