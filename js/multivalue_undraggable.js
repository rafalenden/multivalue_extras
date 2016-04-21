(function ($) {

  /**
   * Removes weight ordering option from multiple value field.
   */
  Drupal.behaviors.multivalueExtrasUndraggable = {
    attach: function (context, settings) {
      var includedFields = settings.multivalue_extras_undraggable_fields;

      $.each(includedFields, function(wrapperClass, fieldLabel) {
        $('.' + wrapperClass + ' table.tabledrag-processed', context).each(function() {
          var $draggableTable = $(this);
          $draggableTable.removeClass('tabledrag-processed');
          $('.' + wrapperClass, context).find('.tabledrag-toggle-weight-wrapper').remove();
          $draggableTable.find('.field-multiple-drag, .tabledrag-hide').remove();
        });
      });
    }
  };

}(jQuery));
