/*
 * 
 */

package de.naoth.naoscp;

import java.util.regex.Pattern;

/**
 *
 * @author Heinrich Mellmann
 * inspired by http://www.mkyong.com/regular-expressions/how-to-validate-ip-address-with-regular-expression
 */
public class IPAddressValidator {
    
    private static final String IPADDRESS_PATTERN = 
		"^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\." +
		"([01]?\\d\\d?|2[0-4]\\d|25[0-5])$";
    
    private static final Pattern pattern = Pattern.compile(IPADDRESS_PATTERN);
 
    private IPAddressValidator(){
    }
 
   /**
    * Validate ip address with regular expression
    * @param ip ip address for validation
    * @return true valid ip address, false invalid ip address
    */
    public static boolean validate(final String ip){
	  return pattern.matcher(ip).matches();	    	    
    }
}
