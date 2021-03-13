var _global_text_process_result = '';

function getStringFromJS(targetStr, msg)
{
    console.log(msg + ":" + targetStr);
    var jsString = targetStr;
    var lengthBytes = lengthBytesUTF8(jsString)+1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
    console.log(msg + " heap:" + stringOnWasmHeap);

    return stringOnWasmHeap;
}

function multiLinesPrompt(inputString, inputMode, inputTitle)
{
    bootbox.prompt({        
	    title: inputTitle,
        inputType: "textarea",
        value: inputString,
                
        callback: function (result) { console.log(result); _global_text_process_result = result; }
    });
}

function globalTextProcessResultIsValid()
{
    return _global_text_process_result !== ""
}

function getGlobalTextProcessResult()
{    
	results = getStringFromJS(_global_text_process_result, "js getGlobalTextProcessResult")
	_global_text_process_result = ""
    
    return results	
}
