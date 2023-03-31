import { Directive, EventEmitter, Input, Output } from '@angular/core';

@Directive({
  selector: '[init]',
  exportAs: 'init',
})
export class OnInitDirective {
  @Output() init: EventEmitter<void> = new EventEmitter();

  constructor() {}

  ngAfterViewInit() {
    setTimeout(() => this.init.next(), 0);
  }
}
