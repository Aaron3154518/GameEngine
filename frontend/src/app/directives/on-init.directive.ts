import { Directive, Input } from '@angular/core';

@Directive({
  selector: '[init]',
  exportAs: 'init',
})
export class OnInitDirective {
  @Input() args: any[] = [];
  @Input() onInit: (...args: any[]) => void = () => {};

  constructor() {}

  ngOnInit() {
    this.onInit(...this.args);
  }
}
