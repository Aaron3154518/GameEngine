import {
  Component,
  ElementRef,
  EventEmitter,
  Input,
  OnChanges,
  Output,
  SimpleChange,
  SimpleChanges,
  ViewChild,
} from '@angular/core';

@Component({
  selector: 'app-input',
  templateUrl: './input.component.html',
  styleUrls: ['./input.component.css'],
})
export class InputComponent {
  @Input() classes: string[] = [];
  @Input() placeholder: string = '';
  @Input() outline: boolean = true;

  @Input() sanitize: (value: string) => string = (s: string) => s;

  @Output() enter: EventEmitter<string> = new EventEmitter();

  @ViewChild('input', { static: true }) input?: ElementRef<HTMLInputElement>;
  @ViewChild('dummy', { static: true }) dummy?: ElementRef<HTMLSpanElement>;

  protected _value: string = '';
  @Input() set value(s: string) {
    this._value = s;
    setTimeout(() => {
      if (this.input && this.dummy) {
        this.sizeInput(this.input.nativeElement, this.dummy.nativeElement);
      }
    }, 0);
  }
  get value(): string {
    return this._value;
  }
  @Output() valueChange: EventEmitter<string> = new EventEmitter();

  select() {
    this.input?.nativeElement.focus();
  }

  sizeInput(input: HTMLInputElement, dummy: HTMLSpanElement) {
    let txt: string = input.value ? input.value : input.placeholder;
    dummy.innerHTML = txt.replaceAll(' ', '&nbsp;');
    input.style.width = `${dummy.offsetWidth}px`;
  }

  onInput(input: HTMLInputElement, dummy: HTMLSpanElement) {
    this.value = input.value = this.sanitize(input.value);
    this.valueChange.next(this.value);
    this.sizeInput(input, dummy);
  }

  onKeyPress(
    event: KeyboardEvent,
    input: HTMLInputElement,
    dummy: HTMLSpanElement
  ) {
    if (event.key === 'Enter' && input.value) {
      this.enter.next(input.value);
      input.value = '';
      this.sizeInput(input, dummy);
    }
  }
}
