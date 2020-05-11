let alert = '<div class="alert alert-warning" role="alert">'+
  'Seems like if your device/browser do not support accelerometer. Try to change device/browser.'+
  '</div>';

$(document).ready(function() {
  try {
    // Create Sensor
    let sensor = new Accelerometer({frequency:1});
    sensor.onerror = event => console.log(event.error.name, event.error.message);
    $('#stop').click( () =>{sensor.stop()} );
    $('#start').click( () =>{sensor.start()} );
    
    sensor.onreading = () => {
        console.log("Acceleration along X-axis: " + sensor.x);
        $('#x').text(sensor.x);
        console.log("Acceleration along Y-axis: " + sensor.y);
        $('#y').text(sensor.y);
        console.log("Acceleration along Z-axis: " + sensor.z);
        $('#z').text(sensor.z);
    }
    
    //sensor.start()
    
    //eventually add: sensor.stop()


    } catch(error) {
        console.log('Error creating sensor:')
        console.log(error);
        $('body').append($.parseHTML(alert));
        setTimeout( ()=>{$().alert('close')}, 2000 );
        //Fallback, do something else etc.
    }

   
  
});


