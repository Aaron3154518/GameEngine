import {
  Component,
  ElementRef,
  EventEmitter,
  Input,
  Output,
  ViewChild,
} from '@angular/core';

@Component({
  selector: 'app-input',
  templateUrl: './input.component.html',
  styleUrls: ['./input.component.css'],
})
export class InputComponent {
  @Input() id: string = '';
  @Input() classes: string[] = [];
  @Input() placeholder: string = '';
  @Input() outline: boolean = true;

  @Input() sanitize: (value: string) => string = (s: string) => s;

  @Output() input: EventEmitter<void> = new EventEmitter();
  @Output() enter: EventEmitter<string> = new EventEmitter();

  @ViewChild('input', { static: true })
  inputRef?: ElementRef<HTMLInputElement>;
  @ViewChild('dummy', { static: true })
  dummyRef?: ElementRef<HTMLSpanElement>;

  protected _value: string = '';
  @Input() set value(s: string) {
    this._value = s;
    if (this.inputRef && this.dummyRef) {
      this.inputRef.nativeElement.value = this.value;
      this.sizeInput(this.inputRef.nativeElement, this.dummyRef.nativeElement);
    }
  }
  get value(): string {
    return this._value;
  }
  @Output() valueChange: EventEmitter<string> = new EventEmitter();

  select() {
    this.inputRef?.nativeElement.focus();
  }

  sizeInput(input: HTMLInputElement, dummy: HTMLSpanElement) {
    let txt: string = input.value ? input.value : input.placeholder;
    dummy.innerHTML = txt.replaceAll(' ', '&nbsp;');
    input.style.width = `${dummy.offsetWidth}px`;
  }

  onInput(inputRef: HTMLInputElement, dummy: HTMLSpanElement) {
    this.value = inputRef.value = this.sanitize(inputRef.value);
    this.valueChange.next(this.value);
    this.sizeInput(inputRef, dummy);
  }

  onKeyPress(event: KeyboardEvent, input: HTMLInputElement) {
    if (event.key === 'Enter' && input.value) {
      this.enter.next(input.value);
    }
  }
}
