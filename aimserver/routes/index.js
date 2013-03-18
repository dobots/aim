
/*
 * GET home page.
 */

exports.index = function(req, res){
  res.render('index', { title: 'Welcome to the AIM Server of the Almende Group' });
};
