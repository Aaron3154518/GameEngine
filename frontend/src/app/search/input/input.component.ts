import { Component, Input, Output } from '@angular/core';

@Component({
  selector: 'app-input',
  templateUrl: './input.component.html',
  styleUrls: ['./input.component.css'],
})
export class InputComponent {
  @Input() classes: string[] = [];
  @Input() inputClasses: string[] = [];
  @Input() placeholder: string = '';

  @Input() enter: (value: string) => void = () => {};

  sizeInput(input: HTMLInputElement, dummy: HTMLSpanElement) {
    let txt: string = input.value ? input.value : input.placeholder;
    dummy.innerHTML = txt.replaceAll(' ', '&nbsp;');
    console.log(dummy.offsetWidth);
    input.style.width = `${dummy.offsetWidth}px`;
  }

  onKeyPress(event: KeyboardEvent, input: HTMLInputElement) {
    if (event.key === 'Enter' && input.value) {
      // this.parameterService.newParamGroup(
      //   new ParameterGroup({
      //     name: input.value,
      //   })
      // );
      this.enter(input.value);
      input.value = '';
    }
  }
}
