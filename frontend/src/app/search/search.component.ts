import { Component, EventEmitter, Input, Output } from '@angular/core';

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css'],
})
export class SearchComponent {
  @Input() data: readonly any[] = [];
  @Input() idCol: string = '';
  @Input() cols: string[] = [];
  @Input() colInputs: { [key: string]: (row: any, val: string) => void } = {};

  @Output() newRow: EventEmitter<string> = new EventEmitter<string>();

  sanitizeVar(s: string): string {
    return `1${s}`
      .replace(RegExp('[^0-9a-zA-Z_]', 'g'), '')
      .replace(RegExp('[0-9]+'), '');
  }

  foo(str: string) {
    console.log(str);
  }
}
