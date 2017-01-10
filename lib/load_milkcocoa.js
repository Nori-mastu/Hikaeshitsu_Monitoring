//全データ
var datastore = [];
var camera1 = [];
var camera2 = [];

// milkcocoa　からデータ獲得
var milkcocoa = new MilkCocoa('uniixkmdl8v.mlkcca.com');

(function () {

  milkcocoa.dataStore('production').stream().size(40).next(function(err, datas) {

    datastore = datas.map(function(d) {
      return {
        timestamp : new Date(d.timestamp),
        num_people : 0,
        co2: Number(d.value.co2),
        tmp : Number(d.value.tmp),
        hum : Number(d.value.hum)
      }
    });
    console.log(datastore);
  });

  milkcocoa.dataStore('camera1').stream().size(40).next(function(err, datas) {

    camera1 = datas.map(function(d) {
      var num = 0;
      if (Number(d.value.num_people) > 0) {
        num = Number(d.value.num_people);
      }
      return {
        timestamp : new Date(d.timestamp),
        num_people : num,
      }
    });
    console.log(camera1);
  });

  milkcocoa.dataStore('camera2').stream().size(40).next(function(err, datas) {

    camera2 = datas.map(function(d) {
      var num = 0;
      if (Number(d.value.num_people) > 0) {
        num = Number(d.value.num_people);
      }
      return {
        timestamp : new Date(d.timestamp),
        num_people : num,
      }
    });
    console.log(camera2);
  });

}());
