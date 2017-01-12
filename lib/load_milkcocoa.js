//全データ
var datastore = [];

// milkcocoa　からデータ獲得
var milkcocoa = new MilkCocoa('uniixkmdl8v.mlkcca.com');

(function () {

  milkcocoa.dataStore('production').stream().size(40).next(function(err, datas) {

    datastore = datas.map(function(d) {
      return {
        timestamp : new Date(d.timestamp),
        num_people : Number(d.value.num_people),
        co2: Number(d.value.co2),
        tmp : Number(d.value.tmp),
        hum : Number(d.value.hum)
      }
    });
    console.log(datastore);
  });


}());
