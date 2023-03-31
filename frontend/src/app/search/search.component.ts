import {
  Component,
  ComponentRef,
  EventEmitter,
  Input,
  Output,
  Type,
  ViewContainerRef,
} from '@angular/core';
import { ContainerDirective } from '../directives/container.directive';

export interface ColComponent {
  data: any;
}

@Component({
  selector: 'col-header',
  template: ` <div
    class="input-group-text d-inline-block text-start py-0 px-1 w-100 border-start-0 rounded-0 rounded-end"
  >
    <span class="align-middle fw-bold">{{ data }}</span>
  </div>`,
})
export class ColHeaderComponent implements ColComponent {
  @Input() data: string = '';
}

export interface Column {
  key: string;
  component: Type<any>;
  input?: (row: any, val: string) => void;
}

@Component({
  selector: 'app-search',
  templateUrl: './search.component.html',
  styleUrls: ['./search.component.css'],
})
export class SearchComponent {
  @Input() data: readonly any[] = [];
  @Input() cols: Column[] = [];
  @Input() allowNew: boolean = false;

  @Output() newRow: EventEmitter<string> = new EventEmitter<string>();

  init(container: ContainerDirective, row: any, col: Column): void {
    const viewContainerRef: ViewContainerRef = container.viewContainerRef;
    viewContainerRef.clear();

    const componentRef: ComponentRef<ColComponent> =
      viewContainerRef.createComponent<ColComponent>(col.component);
    componentRef.instance.data = row[col.key];
  }

  sanitizeVar(s: string): string {
    return `1${s}`
      .replace(RegExp('[^0-9a-zA-Z_]', 'g'), '')
      .replace(RegExp('[0-9]+'), '');
  }
}
