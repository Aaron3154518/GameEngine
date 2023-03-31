import { Directive, ViewContainerRef } from '@angular/core';

@Directive({
  selector: '[appContainer]',
  exportAs: 'container',
})
export class ContainerDirective {
  constructor(public viewContainerRef: ViewContainerRef) {}
}
