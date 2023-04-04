import {
  Component,
  ElementRef,
  EventEmitter,
  Input,
  OnChanges,
  Output,
  SimpleChanges,
  ViewChild,
} from '@angular/core';

@Component({
  selector: 'app-input',
  templateUrl: './input.component.html',
  styleUrls: ['./input.component.css'],
})
export class InputComponent implements OnChanges {
  @Input() id: string = '';
  @Input() placeholder: string = '';
  @Input() outline: boolean = true;
  @Input() passlist: string = '';
  @Input() maxlen: number = 20;
  @Input() data: any;

  @Input() sanitize: (s: string, i: number) => string = (s: string) => s;
  @Input() validate: (s: string) => boolean = () => true;

  @Output() input: EventEmitter<void> = new EventEmitter();
  @Output() enter: EventEmitter<string> = new EventEmitter();

  @ViewChild('input', { static: true })
  inputRef?: ElementRef<HTMLInputElement>;
  @ViewChild('dummy', { static: true })
  dummyRef?: ElementRef<HTMLSpanElement>;

  protected _value: string = '';
  @Input() set value(s: string) {
    this._value = this.passlist
      ? s.replace(RegExp(`[^${this.passlist}]`), '')
      : s;
    this.err = this.value.length > 0 && !this.validate(this.value);
    if (this.inputRef && this.dummyRef) {
      this.inputRef.nativeElement.value = this.value;
      this.sizeInput(this.inputRef.nativeElement, this.dummyRef.nativeElement);
    }
    this.valueChange.next(this.value);
  }
  get value(): string {
    return this._value;
  }
  @Output() valueChange: EventEmitter<string> = new EventEmitter();
  initialValue: string = '';

  @Input() set classes(cls: string | string[]) {
    this._classes = typeof cls === 'string' ? cls : cls.join(' ');
  }
  get classes(): string {
    return this._classes;
  }
  private _classes: string = '';

  protected err: boolean = false;
  get valid(): boolean {
    return this.value.length > 0 && !this.err;
  }

  ngOnChanges(changes: SimpleChanges): void {
    if (changes['value']) {
      this.initialValue = this.value;
    }
  }

  select() {
    this.inputRef?.nativeElement.focus();
  }

  sizeInput(input: HTMLInputElement, dummy: HTMLSpanElement) {
    let txt: string = input.value ? input.value : input.placeholder;
    dummy.innerHTML = txt.replaceAll(' ', '&nbsp;');
    input.style.width = `${dummy.offsetWidth}px`;
  }

  onInput(input: HTMLInputElement) {
    let prevLen: number = input.value.length;
    let i: number = input.selectionStart ? input.selectionStart : 1;
    this.value = input.value;
    if (prevLen !== input.value.length) {
      input.selectionStart = input.selectionEnd = i - 1;
    }
  }

  onKeyPress(event: KeyboardEvent) {
    if (event.key === 'Enter') {
      this.submit();
    }
  }

  submit() {
    if (this.valid && this.value !== this.initialValue) {
      this.enter.next(this.value);
    }
  }
}
