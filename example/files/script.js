var ignore = 0;
var login = 0;
var username;
var password;
function conductorToggle(id){
  ignore++;
  $.ajax ({
    type: "PUT",
    url: '/Conductors',
    datatype: "json",
    beforeSend: function (xhr) {
      xhr.setRequestHeader ("Authorization", "Basic " + btoa(username + ":" + password));
    },
    contentType: "application/json; charset=utf-8",
    data: '{"id":' + id +',"state":' + ($('#conductor' + id).is(':checked') ? '"ON"': '"OFF"') + '}',
    success: function (data) {
      update();
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {
      login = 0;
    }
  });
}

function updateMCBs(){
  $.ajax ({
    type: "GET",
    url: '/MCBs',
    datatype: "json",
    beforeSend: function (xhr) {
      xhr.setRequestHeader ("Authorization", "Basic " + btoa(username + ":" + password));
    },
    success: function (data) {
      $.each(data, function(index){
        $('#mcb'+data[index].id).removeClass('label-danger');
        $('#mcb'+data[index].id).removeClass('label-success');
        if (data[index].state === "ON"){
          $('#mcb'+data[index].id).addClass('label-success');
        }
        else {
          $('#mcb'+data[index].id).addClass('label-danger');
        }
        $("#mcbCurrent"+data[index].id).html(data[index].current);
      });
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {
      login = 0;
    }
  });
}

function updateConductors(){
  $.ajax ({
    type: "GET",
    url: '/Conductors',
    datatype: "json",
    beforeSend: function (xhr) {
      xhr.setRequestHeader ("Authorization", "Basic " + btoa(username + ":" + password));
    },
    success: function (data) {
      $.each(data, function(index){
        if (data[index].state === "ON"){
          $('#conductor'+data[index].id).prop('checked', true);
        }
        else {
          $('#conductor'+data[index].id).prop('checked', false);
        }
      });
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {
      login = 0;
    }
  });
}

function update()
{
  if (login !== 0){
    if (ignore > 0){
      ignore--;
    }
    else {
      updateMCBs();
      updateConductors();
    }
    setTimeout(update, 5000);
  }
  else {
    $('#loginModal').modal('show');
  }
}

function saveCred()
{
  username = $("#usr").val();
  password = $("#pwd").val();
  $.ajax ({
    type: "GET",
    url: '/MCBs',
    datatype: "json",
    beforeSend: function (xhr) {
      xhr.setRequestHeader ("Authorization", "Basic " + btoa(username + ":" + password));
    },
    success: function (data) {
      $('#loginModal').modal('hide');
      login = 1;
      update();
    },
    error: function(XMLHttpRequest, textStatus, errorThrown) {

    }
  });
}

$(window).on('load',function(){
  $("#loginModal").modal('show');
});
