import { Component, Input } from '@angular/core';

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css'],
})
export class SearchComponent {
  @Input() data: readonly any[] = [];
  @Input() idCol: string = '';
  @Input() cols: string[] = [];

  foo(str: string) {
    console.log(str);
  }
}
