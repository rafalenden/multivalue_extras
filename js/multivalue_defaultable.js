(function ($) {

  /**
   * Convert draggable table for weight ordering into a links used to set the default value (lowest weight).
   */
  Drupal.behaviors.multivalueExtrasDefaultable = {
    attach: function (context, settings) {
      var includedFields = settings.multivalue_extras_defaultable_fields;

      $.each(includedFields, function(wrapperClass, fieldLabel) {
        $('.' + wrapperClass + ' table.tabledrag-processed', context).each(function() {
          var $draggableTable = $(this);
          if (typeof Drupal.multivalueDefaultable[$draggableTable.attr('id')] === 'undefined') {
            $draggableTable.once('multivalue_defaultable', function () {
              Drupal.multivalueDefaultable[$draggableTable.attr('id')] = new Drupal.multivalueDefaultable(this, fieldLabel);
            });
          }
        });
      });
    }
  };

  /**
   * Object used to represent draggable table with option to set default item.
   */
  Drupal.multivalueDefaultable = function(table, fieldLabel) {
    var self = this;
    var $table = $(table);

    // Required object variables.
    this.table = table;
    this.fieldLabel = fieldLabel;
    this.defaultRow = null;

    $table.addClass('multivalue-defaultable');

    // Hide default multivalue elements.
    $table.parent().parent().find('.tabledrag-toggle-weight-wrapper').remove();
    $table.find('.field-multiple-drag').remove();
    $table.find('th.tabledrag-hide').remove();
    $table.find('.delta-order').hide().removeClass('tabledrag-hide');

    $('> tr.draggable, > tbody > tr.draggable', table).each(function () {
      self.initRows(this);
    });
  };

  /**
   * Take an item (draggable table row) and add event handlers.
   */
  Drupal.multivalueDefaultable.prototype.initRows = function (item) {
    var self = this,
      $actionColumn = $('<td>').addClass('set-default'),
      $row = $(item),
      index = $row.index(),
      $trigger, $weightElement, $clickedRow, $clickedWeightElement,
      defaultWeight = 0,
      $makeDefaultText = $('<strong>').text(Drupal.t('Default @field', {'@field': self.fieldLabel.toLowerCase()})),
      $makeDefaultLink = $('<a>').attr('href', '#set-default').text(Drupal.t('Set default'));

    // Is this a default item?
    if (index === 0 || !self.defaultRow) {
      $trigger = $makeDefaultText;
      self.defaultRow = $row;
    }
    else {
      $weightElement = self.defaultRow.find('select');
      $trigger = $makeDefaultLink;

      // Change default item.
      $trigger.bind('click', function() {
        $clickedRow = $(this).closest('.draggable');
        $clickedWeightElement = $clickedRow.find('select');

        // Replace weight of select elements.
        defaultWeight = $weightElement.val();
        $weightElement.val($clickedWeightElement.val());
        $clickedWeightElement.val(defaultWeight);

        // Update content of default row trigger and the clicked row trigger.
        self.defaultRow.find('.set-default').html($makeDefaultLink);
        $clickedRow.find('.set-default').html($makeDefaultText);

        self.defaultRow = $clickedRow;
        $weightElement = $clickedWeightElement;

        return false;
      });
    }

    // Add table column with trigger to set the default item.
    $actionColumn.html($trigger);
    $row.append($actionColumn);
  };

}(jQuery));
