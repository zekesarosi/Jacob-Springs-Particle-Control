function doPost(e) {
  // e.parameter.data
  // e.parameter.published_at "date"

  var publishedAt = new Date(e.parameter.published_at);

  var dataArray = [];
  try {
    dataArray = JSON.parse(e.parameter.data);
  }
  catch(e) {
  }

  var sheet = SpreadsheetApp.getActiveSheet();

  var row = [e.parameter.coreid, publishedAt];

  row = row.concat(dataArray);

  sheet.appendRow(row);

  var result = {};
  result.ok = true;

  return ContentService.createTextOutput(JSON.stringify(result))
    .setMimeType(ContentService.MimeType.JSON);
}
