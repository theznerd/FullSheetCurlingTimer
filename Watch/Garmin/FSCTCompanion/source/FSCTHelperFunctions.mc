import Toybox.Lang;

module FSCTHelperFunctions {
    // Replace instances of oldString with newString
    function stringReplace(str as String, oldString as String, newString as String) as String {
        var result = str;
        var index = result.find(oldString);

        while (index != null) {
            var index2 = index + oldString.length();
            result = result.substring(0, index) + newString + result.substring(index2, result.length());
            index = result.find(oldString); // Find the next occurrence of oldString in the modified string
        }
        return result;
    }
}